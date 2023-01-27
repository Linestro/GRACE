#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <random>
#include <cassert>
#include <cstring>
#include "timer.h"
#include <omp.h>
#include <functional>
#include <limits.h>
#include <set>

#define NUM_OF_TABLE 1
#define BATCH_SIZE 64
#define DIMENSION 64

#define CACHE_ITEM_RATIO 1

// #define SAMPLING_PERCENT 10

using namespace std;

bool long_int_cmp (long int i, long int j) { return (i<j); }
bool sw_dict_cmp(pair<long int, long int>& a, pair<long int, long int>& b){
    return a.second != b.second?  a.second > b.second : a.first > b.first;
}
bool edge_cmp(pair<pair<int, int>, long long>&a, pair<pair<int, int>, long long>&b){
    return a.second != b.second?  a.second > b.second : a.first.first > a.first.second;
}

long long orig = 0;
long long actual = 0;

vector<long> table_bounds;

int ret_group(long idx){
    for(int i = 1;  i < 5; i++){
        if(idx < table_bounds[i]){
            return (i - 1);
        }
    }
    assert(0);
    return -1;
}

int main(int argc, char** argv)
{
    long int rowCount = 0;
    map<pair<int, int>, long long> edge_weight;

    std::ifstream infile(argv[1], std::ios::app);
    std::ifstream aux_file(argv[2], std::ios::app);
    std::string line;

    omp_set_num_threads(NUM_OF_TABLE);

    std::map<long int, std::vector<long int>> user_item;
    std::vector<std::vector<long int>> sorted_user_item;
    // std::map<long int, long int> obj_freq;
    long int dict_size = -1;
    long int total_access = 0;
    
    while (std::getline(aux_file, line)){
        std::istringstream iss(line);
        long table_bound;
        if (!(iss >> table_bound)) { break; } // error
        table_bounds.push_back(table_bound);
        // cout << table_bound << endl;
    }
    // return 0;

    while (std::getline(infile, line))
    {   
        std::istringstream iss(line);
        long int usr = 0;
        long int obj = 0;
        char dummy;
        if (!(iss >> usr >> dummy >> obj)) { break; } // error
        
        if (user_item.find(usr) != user_item.end() && 
          std::find(user_item[usr].begin(), user_item[usr].end(), obj) != user_item[usr].end() ){
            continue;
        }

        total_access++;
        
        if (user_item.find(usr) == user_item.end()){
            user_item[usr] = std::vector<long int>();
        }
        user_item[usr].push_back(obj);

        
        // if (obj_freq.find(obj) == obj_freq.end()){
        //     obj_freq[obj] = 0;
        // }
        // obj_freq[obj]++;

        if (obj + 1 > dict_size){
            dict_size = obj + 1;
        }
    }

    // vector<pair<long int, long int> > global_freq;
    // map<long int, long int> space_reorder_ind;
    // for (auto& it : obj_freq) {
    //     global_freq.push_back(it);
    // }
    // sort(global_freq.begin(), global_freq.end(), sw_dict_cmp);

    
    // for (int i = 0; i < int(global_freq.size()); i++){
    //     space_reorder_ind[global_freq[i].first] = i;
    // }
    
    // for (int i = 0; i < int(global_freq.size()); i++){
    //     global_freq[i].first = i;
    // }

    // cout << "dict size is: " << dict_size << endl;
    // cout << "User #: " << user_item.size() << endl;
    // cout << "Item #: " << obj_freq.size() << endl;
    // cout << "Access #: " << total_access << endl;
    // cout << "Avg user accesses #: " << total_access * 1.0 / user_item.size() << endl;
    // cout << "Avg obj being accessed #: " << total_access * 1.0 / obj_freq.size() << endl;

    // vector<vector<int> > cached_ps;
    // map<int, int> item_to_cacheline;
    // int file_line = 0;
    // while (std::getline(graphfile, line)){   
    //     std::istringstream iss(line);
    //     string single_item;
    //     vector<int> tmp;
    //     while (iss >> single_item){
    //         tmp.push_back(std::stoi(single_item));
    //         item_to_cacheline[std::stoi(single_item)] = file_line;
    //     }
    //     cached_ps.push_back(tmp);
    //     file_line++;
    // }

    // for(int i = 0; i < cached_ps.size(); i++){
    //     for(int j = 0; j < cached_ps[i].size(); j++){
    //         cout << cached_ps[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    rowCount = dict_size;

    // set<long> triangle_set;
    // for(int i = 0; i < cutdown; i++){
    //     triangle_set.insert(global_freq[i].first);
    // }

    long iterate = 0;
    for (auto it = user_item.cbegin(); it != user_item.cend(); ++it) {
        assert(it->first == iterate);
        if(iterate >= user_item.size() / 2){
            break;
        }
        iterate++;
        std::vector<long int> myvector (std::begin(it->second), std::end(it->second));        
        // for(int i = 0; i < myvector.size(); i++){
        //     myvector[i] = space_reorder_ind[myvector[i]];
        // }
        
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        vector<vector<long>> sub_vecs;
        for(int i = 0; i < 4; i++){
            sub_vecs.push_back(vector<long>());
        }
        for(int i = 0; i < myvector.size(); i++){
            sub_vecs[ret_group(myvector[i])].push_back(myvector[i]);
        }
        for(int i = 0; i < 4; i++){
            if(sub_vecs[i].size() > 0){
                sorted_user_item.push_back(sub_vecs[i]);
                // for(int j = 0; j < sub_vecs[i].size(); j++){
                //     cout << sub_vecs[i][j] << " ";
                // }
                // cout << endl;
            }
        }
        // return 0;
    }

    


    // std::random_shuffle ( sorted_user_item.begin(), sorted_user_item.end() );
    user_item.clear();

    for (int batch_idx = 0; batch_idx < sorted_user_item.size(); batch_idx++){
        for(int k = 0; k < 1; k++){
        int idx = batch_idx * 1 + k;
        // double r = ((double) rand() / (RAND_MAX));
        // if(r > ((double) SAMPLING_PERCENT / 100.0) ){
        //     continue;
        // }
        orig += sorted_user_item[idx].size();
        // long long in_triangle = 0;
        // long long out_triangle = 0;
        
        set<long> access_set;
        for (int i = 0; i < sorted_user_item[idx].size(); i++){
            for (int j = i; j < sorted_user_item[idx].size(); j++){
                auto this_pair = make_pair(sorted_user_item[idx][i], sorted_user_item[idx][j]);
                if (edge_weight.find(this_pair) == edge_weight.end()){
                    edge_weight[this_pair] = 0;
                }
                edge_weight[this_pair]++;
            }
        }
        // actual += access_set.size();
        // cout << endl;
        }
    }

    vector<pair<pair<int, int>, long long> > edge_weight_vec;
    for(auto &it : edge_weight){
        edge_weight_vec.push_back(it);
    }
    sort(edge_weight_vec.begin(), edge_weight_vec.end(), edge_cmp);

    std::cout << dict_size - 1 << " " 
                << dict_size - 1 << " " 
                << 0 << "\n";
    // std::cout << rowCount << std::endl;
    // std::cout << (long long)(edge_weight_vec.size() * 0.1) << std::endl;
    for (size_t i = 0; i < edge_weight_vec.size() * 0.2; i++){
        std::cout << edge_weight_vec[i].first.first << " " 
                << edge_weight_vec[i].first.second << " " 
                << edge_weight_vec[i].second << "\n";
    }

    // for (size_t i = 0; i < edge_weight_vec.size(); i++){
    //     std::cout << edge_weight_vec[i].first.first << " " 
    //             << edge_weight_vec[i].first.second << " " 
    //             << edge_weight_vec[i].second << "\n";
    // }

    // cout << "Original: " << orig << endl;
    // cout << "Actual: " << actual << endl;
    // cout << "Saving: " << 1.0 * orig / actual << endl;
    
    return 0;

}