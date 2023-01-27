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
#include <immintrin.h>

#define DIMENSION 1024
#define USER_BATCH_SIZE 1024 

#define OPENMP_THREAD 128

using namespace std;


long hbm_emb_entries;
long max_reduction_size;



bool sw_dict_cmp(pair<long int, long int>& a,
         pair<long int, long int>& b)
{
    return a.second != b.second?  a.second > b.second : a.first > b.first;
}

bool long_int_cmp (long int i, long int j) { return (i<j); }
bool vector_cmp(vector<long>& a, vector<long>& b){
    return a.size() > b.size();
}
long space_addr_gen(long a, long b, long thres, long addr_offset)
{
    return addr_offset + (long)(a * (2 * thres - a - 1) / 2 + (b - a - 1));
}

// map<long, long> save_detail;

int main( int argc, char* argv[] )
{
    float single_cutdown = 0.0;
	omp_set_num_threads(OPENMP_THREAD);
    // std::ifstream hbm_log(argv[1], std::ios::app);
    // std::ifstream ddr_log(argv[2], std::ios::app);
    // std::ifstream graphfile(argv[1], std::ios::app);
    std::ifstream user_trace(argv[2], std::ios::app);
    std::ifstream obj_trace(argv[3], std::ios::app);
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

    for(int hbm_cap_idx = 0; hbm_cap_idx < hbm_cap_group.size(); hbm_cap_idx += 1){
        string workload(argv[1]);
        string prefix = workload + "_" + hbm_cap_group[hbm_cap_idx];
        prefix_group.push_back(make_pair(prefix, stof(hbm_cap_group[hbm_cap_idx])));
        // cout << prefix << endl;
    }

    std::vector<std::vector<long int>> user_item;
    std::vector<std::vector<long int>> sorted_user_item;
    std::map<long int, long int> obj_freq;
    long total_access = 0;
    std::string line;
    long user_size, item_size;
    std::getline(user_trace, line);
    std::istringstream iss(line);
    
    
    std::map<long int, std::vector<long int>> user_item_map;
    while (std::getline(obj_trace, line))
    {   
        std::istringstream iss(line);
        long int usr = 0;
        long int obj = 0;
        char dummy;
        if (!(iss >> usr >> dummy >> obj)) { break; } // error
        if (user_item_map.find(usr) != user_item_map.end() && 
          std::find(user_item_map[usr].begin(), user_item_map[usr].end(), obj) != user_item_map[usr].end() ){
            continue;
        }
        if (user_item_map.find(usr) == user_item_map.end()){
            user_item_map[usr] = std::vector<long int>();
        }
        user_item_map[usr].push_back(obj);
    }
    assert(user_item_map.size() > 0);
    std::vector<std::vector<long int>> sorted_user_item_obj;
    for (auto it = user_item_map.cbegin(); it != user_item_map.cend(); ++it) {
        std::vector<long int> myvector (std::begin(it->second), std::end(it->second));
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        sorted_user_item_obj.push_back(myvector);
    }
    for(long i = 0; i < sorted_user_item_obj.size() / 2; i++){
        // double r = ((double) rand() / (RAND_MAX));
        // // std::cout << "Random seed is: " << r << std::endl;
        // if(50 < 100 && r > ((double) 50 / 100.0) ){
        //     continue;
        // }
        for(int j = 0; j < sorted_user_item_obj[i].size(); j++){
            long obj = sorted_user_item_obj[i][j];            
            if (obj_freq.find(obj) == obj_freq.end()){
                obj_freq[obj] = 0;
            }
            obj_freq[obj]++;
        }
    }
    
    iss >> user_size >> item_size >> max_reduction_size;
    while (std::getline(user_trace, line))
    {   
        std::istringstream iss(line);
        string single_item;
        vector<long> tmp;
        while (iss >> single_item){
            // if (stol(single_item) + 1 > item_size){
            //     item_size = stol(single_item) + 1;
            // }
            // if (obj_freq.find(stol(single_item)) == obj_freq.end()){
            //     obj_freq[stol(single_item)] = 0;
            // }
            // obj_freq[stol(single_item)]++;
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

    std::ofstream out("hbm_only_space_log/" + (prefix_group[prefix_idx]).first + ".log");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf());

    float hbm_cap = (prefix_group[prefix_idx]).second;

    map<long, long> space_reorder_ind;
    vector<pair<long, long> > A;
    for (auto& it : obj_freq) {
        A.push_back(it);
    }
    sort(A.begin(), A.end(), sw_dict_cmp);
    for (int i = 0; i < int(A.size()); i++){
        space_reorder_ind[A[i].first] = i;
        // cout << "Rank # " << i << " : " << A[i].first << endl;
    }
    
    for (int k = 0; k < user_item.size(); k++) {
        std::vector<long int> myvector (std::begin(user_item[k]), std::end(user_item[k]));
        for(int i = 0; i < myvector.size(); i++){
            myvector[i] = space_reorder_ind[myvector[i]];
        }
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        // myvector.resize(min(int(myvector.size()), 80));
        sorted_user_item.push_back(myvector);
    }

    // assert(obj_freq.size() == item_size);
    
    long ddr_starting_line = 0;
    

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
    
    long psum2space = (long)((hbm_cap - single_cutdown) * item_size);
    long psum2_cutdown = (long)(sqrt(2) * sqrt(psum2space));
    cout << "single cutdown: " << single_cutdown << endl;
    cout << "psum 2 cutdown: " << psum2_cutdown << endl;

    cout << "Cluster total size is: " << psum2_cutdown * (psum2_cutdown + 1) / 2 << endl;
    cout << "Total cluster size vs. item size: " << psum2_cutdown * (psum2_cutdown + 1) / 2 * 1.0 / item_size << endl;
    // assert(psum2_cutdown * (psum2_cutdown - 1) / 2 * 1.0 / item_size <= hbm_cap);

    long hbm_line = (long) (single_cutdown * item_size) + (long) (psum2_cutdown * (psum2_cutdown + 1) / 2) + 1;
    long ddr_emb_entries = (long) (1.0 * item_size);
    hbm_emb_entries = hbm_line + ddr_emb_entries;

    // volatile float *flush_cache = (float*)malloc(120 * 1024 * 1024 / 4 * sizeof(float));  // size of caliber L3 is 120 MB
    float flush_cache_time = 0;
    
    vector<float> ddr_time_log;
    vector<float> hbm_time_log;

    for (long batch_idx = 0; batch_idx < min(int((user_item.size() - 1) / USER_BATCH_SIZE), 16384); batch_idx++){
        vector<vector<long> > raw_accesses_hbm(USER_BATCH_SIZE,vector<long>(max_reduction_size));
        vector<vector<long> > raw_accesses_ddr(USER_BATCH_SIZE,vector<long>(max_reduction_size));
        cout << "Inference on batch # " << batch_idx << endl;
        #pragma omp parallel for reduction(+:pre_compression_ddr) reduction(+:post_compression_ddr) reduction(+:pre_compression_hbm) reduction(+:post_compression_hbm) reduction(+:ddr_count) reduction(+:hbm_count) reduction(+:actual)
        for(int k = 0; k < USER_BATCH_SIZE; k++){
            int idx = batch_idx * USER_BATCH_SIZE + k;
            
            orig += sorted_user_item[idx].size();
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
            vector<long> combined_addr;

            for (int i = 0; i < sorted_user_item[idx].size(); i++){
                long elt = sorted_user_item[idx][i];
                if(i + 1 >= sorted_user_item[idx].size()){
                    combined_addr.push_back(sorted_user_item[idx][i]);
                    continue;
                }
                if(sorted_user_item[idx][i] <= psum2_cutdown && 
                    sorted_user_item[idx][i+1] <= psum2_cutdown ){
                        long inserted_hbm_addr = space_addr_gen(sorted_user_item[idx][i], 
                            sorted_user_item[idx][i+1], 
                            psum2_cutdown, ddr_emb_entries + (long) (single_cutdown * item_size));
                        assert(inserted_hbm_addr < hbm_emb_entries);
                        combined_addr.push_back(inserted_hbm_addr);
                }
                else{
                    combined_addr.push_back(sorted_user_item[idx][i]);
                    combined_addr.push_back(sorted_user_item[idx][i+1]);
                    // ddr_access_addr.push_back(sorted_user_item[idx][i]);
                    // ddr_access_addr.push_back(sorted_user_item[idx][i+1]);
                }
                i++;
            }

            for(int i = 0; i < combined_addr.size(); i++){
                if(combined_addr[i] < (long)(single_cutdown * item_size)){
                    hbm_access_addr.push_back(combined_addr[i]);
                    pre_compression_hbm++;
                    post_compression_hbm++;
                }
                else if(combined_addr[i] < ddr_emb_entries){
                    ddr_access_addr.push_back(combined_addr[i]);
                    pre_compression_ddr++;
                    post_compression_ddr++;
                }
                else{
                    hbm_access_addr.push_back(combined_addr[i] - ddr_emb_entries);
                    pre_compression_hbm+=2;
                    post_compression_hbm++;
                }
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
    cout << "SPACE access: " << post_compression_hbm + post_compression_ddr << endl;
    cout << "Reduction Factor is: " << (pre_compression_hbm + pre_compression_ddr) * 1.0 / (post_compression_hbm + post_compression_ddr) << endl;

    }
 
    return 0;

}