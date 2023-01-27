#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include "time.h"
#include <omp.h>
#include <algorithm>
#include <map>

#define DIMENSION 1024
#define USER_BATCH_SIZE 1024 

#define OPENMP_THREAD 128

using namespace std;

long hbm_emb_entries;
long max_reduction_size;



bool long_int_cmp (long int i, long int j) { return (i<j); }
bool vector_cmp(vector<long>& a, vector<long>& b){
    return a.size() > b.size();
}
// map<long, long> save_detail;

int main( int argc, char* argv[] )
{
    float idle_lat = 0;
	omp_set_num_threads(OPENMP_THREAD);
    // std::ifstream hbm_log(argv[1], std::ios::app);
    // std::ifstream ddr_log(argv[2], std::ios::app);
    std::ifstream user_trace(argv[2], std::ios::app);
    cout << "DIMENSION is: " << DIMENSION << endl;
    cout << "USER_BATCH_SIZE is: " << USER_BATCH_SIZE << endl;
    cout << "OPENMP is: " << OPENMP_THREAD << endl;
    // cout << "HBM cap ratio is: " << hbm_cap << endl;
    // assert(hbm_cap >= 0.0);

    vector<string> hbm_cap_group;
    hbm_cap_group.push_back("1.0");     // HBM only is the same for 0.5, 1.0, etc. 
    // hbm_cap_group.push_back("0.5");
    // hbm_cap_group.push_back("0.25");
    // hbm_cap_group.push_back("0");
    vector<pair<string, float>> prefix_group;

    string workload(argv[1]);
    for(int hbm_cap_idx = 0; hbm_cap_idx < hbm_cap_group.size(); hbm_cap_idx += 1){
        string prefix = workload + "_patoh_hbm_" + hbm_cap_group[hbm_cap_idx];
        prefix_group.push_back(make_pair(prefix, stof(hbm_cap_group[hbm_cap_idx])));
        // cout << prefix << endl;
    }

    std::vector<std::vector<long int>> user_item;
    std::map<long int, long int> obj_freq;
    long total_access = 0;
    std::string line;
    long user_size, item_size;
    std::getline(user_trace, line);
    std::istringstream iss(line);
    iss >> user_size >> item_size >> max_reduction_size;
    while (std::getline(user_trace, line))
    {   
        std::istringstream iss(line);
        string single_item;
        vector<long> tmp;
        while (iss >> single_item){
            total_access++;
            tmp.push_back(std::stol(single_item));
        }
        user_item.push_back(tmp);
    }
    // cout << "dict size is: " << item_size << endl;
    // cout << "User #: " << user_item.size() << endl;
    // // cout << "Item #: " << obj_freq.size() << endl;
    // cout << "Access #: " << total_access << endl;
    // cout << "Avg user accesses #: " << total_access * 1.0 / user_item.size() << endl;
    // cout << "Avg obj being accessed #: " << total_access * 1.0 / item_size << endl;
    // cout << "max_reduction_size: " << max_reduction_size << endl;
    
    // assert(obj_freq.size() == item_size);
    
    for(int prefix_idx = 0; prefix_idx < prefix_group.size(); prefix_idx++){

    vector<long> cluster_starting_addr; 
    long long pre_compression_hbm = 0;
    long long post_compression_hbm = 0;
    long long pre_compression_ddr = 0;
    long long post_compression_ddr = 0;
    long long orig = 0;
    long long ddr_count = 0;
    long long hbm_count = 0;
    long long actual = 0;

    std::ofstream out("hbm_only_metis_log/" + (prefix_group[prefix_idx]).first + ".log");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf());
    std::ifstream graphfile;
    if ((prefix_group[prefix_idx]).second == 0){
        graphfile.open("blank.cluster", std::ios::app);
    }
    else{
        graphfile.open("./metis/metis-5.1.0/cluster/" + workload + "_hbm_1.0.cluster", std::ios::app);
    }


    long ddr_starting_line = 0;
    float addr_gen_time = 0;
    float buffer_reset_time = 0;
    
    float hbm_cap = (prefix_group[prefix_idx]).second;
    long hbm_line = (long)(1.0 * item_size);

    vector<vector<long> > cached_ps;

    map<long, long> item_to_cacheline;
    map<long, long> original_item_to_cacheline;

    map<vector<long>, long> orig_cluster;
    map<long, long> cluster_remap;

    vector<int> is_cached;
    is_cached.resize(item_size, 0);
    cluster_starting_addr.push_back(0);

    int file_line = 0;
    long freq_item = 0;
    
    while (std::getline(graphfile, line)){   
        std::istringstream iss(line);
        string single_item;
        vector<long> tmp;
        while (iss >> single_item){
            // cout << "Single item: " << single_item << endl;
            tmp.push_back(std::stol(single_item));
            original_item_to_cacheline[std::stoi(single_item)] = file_line;
            is_cached[std::stol(single_item)] = true;
        }
        // save_detail[file_line] = 0;
        cached_ps.push_back(tmp);
        orig_cluster[tmp] = file_line;
        file_line++;
        freq_item += tmp.size();
    }

    sort(cached_ps.begin(), cached_ps.end(), vector_cmp);
    for(size_t i = 0; i < cached_ps.size(); i++){
        for(size_t j = 0; j < cached_ps[i].size(); j++){
            // cout << cached_ps[i][j] << " ";
            item_to_cacheline[cached_ps[i][j]] = i;
        }
        cluster_remap[orig_cluster[cached_ps[i]]] = i;
        cluster_starting_addr.push_back(cluster_starting_addr[cluster_starting_addr.size() - 1] + (long)((pow(2.0, cached_ps[i].size()) - 1)));
        ddr_starting_line += (long)((pow(2.0, cached_ps[i].size()) - 1));
        // cout << endl;
    }

    // hbm_line = max(hbm_line, ddr_starting_line);
    cout << "Cluster total size is: " << ddr_starting_line << endl;
    cout << "Total cluster size vs. item size: " << (ddr_starting_line) * 1.0 / item_size << endl;


    hbm_emb_entries = (long) (2.0 * item_size);
    
    vector<float> ddr_time_log;
    vector<float> hbm_time_log;

    for (long batch_idx = 0; batch_idx < min(int((user_item.size() - 1) / USER_BATCH_SIZE), 16384); batch_idx++){
        vector<vector<long> > raw_accesses_hbm(USER_BATCH_SIZE,vector<long>(max_reduction_size));
        vector<vector<long> > raw_accesses_ddr(USER_BATCH_SIZE,vector<long>(max_reduction_size));
        cout << "Inference on batch # " << batch_idx << endl;
        #pragma omp parallel for reduction(+:pre_compression_ddr) reduction(+:post_compression_ddr) reduction(+:pre_compression_hbm) reduction(+:post_compression_hbm) reduction(+:ddr_count) reduction(+:hbm_count) reduction(+:actual)
        for(int k = 0; k < USER_BATCH_SIZE; k++){
            int idx = batch_idx * USER_BATCH_SIZE + k;
            
            orig += user_item[idx].size();
            // vector<vector<long> > hbm_access_set;
            // hbm_access_set.resize(cached_ps.size());

            map<long, vector<long> > hbm_access_set;
            map<long, vector<long> > ddr_access_set;
            // for(int i = 0; i < file_line; i++){
            //     hbm_access_set.push_back(vector<long>());
            // }
            vector<long> hbm_access_addr;    
            vector<long> ddr_access_addr;

            // map<long, bool> seen_before;

            for (int i = 0; i < user_item[idx].size(); i++){
                long elt = user_item[idx][i];
                if(is_cached[elt] == 0){
                    ddr_access_addr.push_back(elt);
                    pre_compression_ddr++;
                    post_compression_ddr++;
                    continue;
                }

                long index = item_to_cacheline[elt];

                for(int j = 0; j < cached_ps[index].size(); j++){
                    if(elt == cached_ps[index][j]){
                        hbm_access_set.insert(make_pair(index, std::vector<long>()));
                        hbm_access_set[index].push_back(j);
                        break;
                    }
                }
            }
            for (auto it = hbm_access_set.begin(); it != hbm_access_set.end(); it++){
                long this_starting_addr = cluster_starting_addr[it->first];
                for(int j = 0; j < (it->second).size(); j++){
                    // assert(this_starting_addr + (long)(pow(2.0, (it->second)[j])) < hbm_line);
                    pre_compression_hbm++;
                }
                for(int j = 0; j < (it->second).size(); j++){
                    this_starting_addr += (long)(pow(2.0, (it->second)[j]));
                }
                // assert(this_starting_addr < hbm_line);
                hbm_access_addr.push_back(this_starting_addr);
                post_compression_hbm++;
            }
            
            for(int i = 0; i < ddr_access_addr.size(); i++){
                hbm_access_addr.push_back(ddr_access_addr[i] + hbm_line);
            }
            ddr_access_addr.clear();

            ddr_count += ddr_access_addr.size();
            hbm_count += hbm_access_addr.size();

            raw_accesses_hbm[k] = hbm_access_addr;
            raw_accesses_ddr[k] = ddr_access_addr;
        }

    }
    
    cout << "Original access: " << pre_compression_hbm + pre_compression_ddr << endl;
    cout << "Metis access: " << post_compression_hbm + post_compression_ddr << endl;
    cout << "Reduction Factor is: " << (pre_compression_hbm + pre_compression_ddr) * 1.0 / (post_compression_hbm + post_compression_ddr) << endl;

    }
 
    return 0;

}