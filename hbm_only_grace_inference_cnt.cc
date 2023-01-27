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


int main( int argc, char* argv[] )
{
        
	omp_set_num_threads(OPENMP_THREAD);
    std::ifstream user_trace(argv[2], std::ios::app);
    cout << "DIMENSION is: " << DIMENSION << endl;
    cout << "USER_BATCH_SIZE is: " << USER_BATCH_SIZE << endl;
    cout << "OPENMP is: " << OPENMP_THREAD << endl;
    // cout << "HBM cap ratio is: " << hbm_cap << endl;
    // assert(hbm_cap >= 0.0);

    vector<string> hbm_cap_group;
    hbm_cap_group.push_back("1.0");
    // hbm_cap_group.push_back("0.5");
    // hbm_cap_group.push_back("0.25");

    vector<pair<string, float>> prefix_group;

    string workload = string(argv[1]);
    int decay = stoi(argv[3]);
    int length = stoi(argv[4]);

    string prefix = workload + "_decay_" + to_string(decay) + "_sampling_50_alpha_50_length_" + to_string(length) + "_hbm_" + "1.0";
    prefix_group.push_back(make_pair(prefix, stof("1.0")));

    // for(int decay = 0; decay <= 80; decay += 10){
    //     for(int length = 1; length <= 8; length += 1){
    //         for(int hbm_cap_idx = 0; hbm_cap_idx < hbm_cap_group.size(); hbm_cap_idx += 1){
    //             string workload(argv[1]);
    //             string prefix = workload + "_decay_" + to_string(decay) + "_sampling_50_alpha_50_length_" + to_string(length) + "_hbm_" + hbm_cap_group[hbm_cap_idx];
    //             prefix_group.push_back(make_pair(prefix, stof(hbm_cap_group[hbm_cap_idx])));
    //             // cout << prefix << endl;
    //         }
    //     }
    // }
    

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

    std::ofstream out("hbm_only_grace_log/" + (prefix_group[prefix_idx]).first + ".log");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf());
    std::ifstream hbm_graphfile("grace_cluster/" + (prefix_group[prefix_idx]).first + ".hbm.cluster", std::ios::app);
    std::ifstream ddr_graphfile("grace_cluster/" + (prefix_group[prefix_idx]).first + ".ddr.cluster", std::ios::app);

    long ddr_starting_line = 0;
    float addr_gen_time = 0;
    
    float hbm_cap = (prefix_group[prefix_idx]).second;
    long hbm_line = (long)(hbm_cap * item_size);

    vector<vector<long> > hbm_cached_ps;
    vector<vector<long> > ddr_cached_ps;

    map<long, long> item_to_cacheline;
    map<long, long> original_item_to_cacheline;

    map<vector<long>, long> orig_cluster;
    map<long, long> cluster_remap;

    vector<int> is_cached;
    is_cached.resize(item_size, 0);
    cluster_starting_addr.push_back(0);

    int file_line = 0;
    long freq_item = 0;
    while (std::getline(hbm_graphfile, line)){   
        std::istringstream iss(line);
        string single_item;
        vector<long> tmp;
        while (iss >> single_item){
            tmp.push_back(std::stol(single_item));
            original_item_to_cacheline[std::stoi(single_item)] = file_line;
            is_cached[std::stol(single_item)] = 1;
        }
        // save_detail[file_line] = 0;
        hbm_cached_ps.push_back(tmp);
        orig_cluster[tmp] = file_line;
        file_line++;
        freq_item += tmp.size();
    }

    sort(hbm_cached_ps.begin(), hbm_cached_ps.end(), vector_cmp);
    for(size_t i = 0; i < hbm_cached_ps.size(); i++){
        for(size_t j = 0; j < hbm_cached_ps[i].size(); j++){
            // cout << cached_ps[i][j] << " ";
            item_to_cacheline[hbm_cached_ps[i][j]] = i;
        }
        cluster_remap[orig_cluster[hbm_cached_ps[i]]] = i;
        cluster_starting_addr.push_back(cluster_starting_addr[cluster_starting_addr.size() - 1] + (long)((pow(2.0, hbm_cached_ps[i].size()) - 1)));
        ddr_starting_line += (long)((pow(2.0, hbm_cached_ps[i].size()) - 1));
        // cout << endl;
    }

    int hbm_num_of_clusters = hbm_cached_ps.size();

    long hbm_total_cap = ddr_starting_line;
    hbm_line = max(hbm_line, ddr_starting_line);
    ddr_starting_line = hbm_line;
    cluster_starting_addr.pop_back();
    cluster_starting_addr.push_back(hbm_line);
    while (std::getline(ddr_graphfile, line)){   
        std::istringstream iss(line);
        string single_item;
        vector<long> tmp;
        while (iss >> single_item){
            tmp.push_back(std::stol(single_item));
            original_item_to_cacheline[std::stoi(single_item)] = file_line;
            is_cached[std::stol(single_item)] = 2;
        }
        // save_detail[file_line] = 0;
        ddr_cached_ps.push_back(tmp);
        orig_cluster[tmp] = file_line;
        file_line++;
        freq_item += tmp.size();
    }

    sort(ddr_cached_ps.begin(), ddr_cached_ps.end(), vector_cmp);
    for(size_t i = 0; i < ddr_cached_ps.size(); i++){
        for(size_t j = 0; j < ddr_cached_ps[i].size(); j++){
            // cout << cached_ps[i][j] << " ";
            item_to_cacheline[ddr_cached_ps[i][j]] = i + hbm_num_of_clusters;
        }
        cluster_remap[orig_cluster[ddr_cached_ps[i]]] = i + hbm_num_of_clusters;
        cluster_starting_addr.push_back(cluster_starting_addr[cluster_starting_addr.size() - 1] + (long)((pow(2.0, ddr_cached_ps[i].size()) - 1)));
        ddr_starting_line += (long)((pow(2.0, ddr_cached_ps[i].size()) - 1));
        // cout << endl;
    }
    long ddr_total_cap = ddr_starting_line - hbm_line;

    cout << "Cluster total size is: " << ddr_total_cap + hbm_total_cap << endl;
    cout << "Total size (cluster total + infreq total): " << (item_size - freq_item + ddr_total_cap + hbm_total_cap) << endl;
    cout << "Total size (cluster total + infreq total) vs. item size: " << (item_size - freq_item + ddr_total_cap + hbm_total_cap) * 1.0 / item_size << endl;


    hbm_emb_entries = (long) hbm_line;
    long ddr_emb_entries = ddr_starting_line - hbm_line + 1; // max(ddr_starting_line - hbm_line, (long) (1.0 * item_size)) + 1;
    hbm_emb_entries += ddr_emb_entries;


    for (long batch_idx = 0; batch_idx < min(int((user_item.size() - 1) / USER_BATCH_SIZE), 16384); batch_idx++){
        vector<vector<long> > raw_accesses_hbm(USER_BATCH_SIZE,vector<long>(max_reduction_size));
        vector<vector<long> > raw_accesses_ddr(USER_BATCH_SIZE,vector<long>(max_reduction_size));
        cout << "Inference on batch # " << batch_idx << endl;

        #pragma omp parallel for reduction(+:pre_compression_ddr) reduction(+:post_compression_ddr) reduction(+:pre_compression_hbm) reduction(+:post_compression_hbm) reduction(+:ddr_count) reduction(+:hbm_count) 
        for(int k = 0; k < USER_BATCH_SIZE; k++){
            int idx = batch_idx * USER_BATCH_SIZE + k;
            
            orig += user_item[idx].size();

            map<long, vector<long> > hbm_access_set;
            map<long, vector<long> > ddr_access_set;
            
            vector<long> hbm_access_addr;    
            vector<long> ddr_access_addr;

            // map<long, bool> seen_before;

            for (int i = 0; i < user_item[idx].size(); i++){
                long elt = user_item[idx][i];
                assert(is_cached[elt] > 0);
                long index = item_to_cacheline[elt];


                if(index < hbm_num_of_clusters){
                    for(int j = 0; j < hbm_cached_ps[index].size(); j++){
                        if(elt == hbm_cached_ps[index][j]){
                            hbm_access_set.insert(make_pair(index, std::vector<long>()));
                            hbm_access_set[index].push_back(j);
                            break;
                        }
                    }
                }
                else{
                    for(int j = 0; j < ddr_cached_ps[index - hbm_num_of_clusters].size(); j++){
                        if(elt == ddr_cached_ps[index - hbm_num_of_clusters][j]){
                            ddr_access_set.insert(make_pair(index, std::vector<long>()));
                            ddr_access_set[index].push_back(j);
                            break;
                        }
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
            
            for (auto it = ddr_access_set.begin(); it != ddr_access_set.end(); it++){
                long this_starting_addr = cluster_starting_addr[it->first];
                for(int j = 0; j < (it->second).size(); j++){
                    pre_compression_ddr++;
                }
                for(int j = 0; j < (it->second).size(); j++){
                    this_starting_addr += (long)(pow(2.0, (it->second)[j]));
                }
                // assert(this_starting_addr >= hbm_line);
                ddr_access_addr.push_back(this_starting_addr);
                post_compression_ddr++;
            }


            for(int i = 0; i < ddr_access_addr.size(); i++){
                hbm_access_addr.push_back(ddr_access_addr[i]);
                if(ddr_access_addr[i] >= hbm_emb_entries){
                    assert(0);
                }
            }
            ddr_access_addr.clear();

            ddr_count += ddr_access_addr.size();
            hbm_count += hbm_access_addr.size();

            raw_accesses_hbm[k] = hbm_access_addr;
            raw_accesses_ddr[k] = ddr_access_addr;
            
        }
    }
    
    // Because of homogeneous HBM only, we only count the number of accesses
    cout << "Original access: " << pre_compression_hbm + pre_compression_ddr << endl;
    cout << "GRACE access: " << post_compression_hbm + post_compression_ddr << endl;
    cout << "Reduction Factor is: " << (pre_compression_hbm + pre_compression_ddr) * 1.0 / (post_compression_hbm + post_compression_ddr) << endl;

    }    
 
    return 0;

}