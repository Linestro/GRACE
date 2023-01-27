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
#include <math.h>
#define NUM_THREADS 128

#define NUM_OF_TABLE 1
#define BATCH_SIZE 64
#define DIMENSION 64

#define CACHE_ITEM_RATIO 1
#define INITIAL_MIN_RATIO 10000
// #define SAMPLING_PERCENT 10

using namespace std;

bool long_int_cmp (long int i, long int j) { return (i<j); }
bool sw_dict_cmp(pair<long int, long int>& a, pair<long int, long int>& b){
    return a.second != b.second?  a.second > b.second : a.first > b.first;
}
bool edge_cmp(pair<pair<int, int>, long long>&a, pair<pair<int, int>, long long>&b){
    return a.second != b.second?  a.second > b.second : a.first.first > a.first.second;
}

bool float_dict_cmp(pair<vector<long>, float>& a,
         pair<vector<long>, float>& b)
{
    return a.second != b.second?  a.second > b.second : a.first.size() > b.first.size();
}

long long orig = 0;
long long actual = 0;

int main(int argc, char** argv)
{
	omp_set_num_threads(NUM_THREADS);
    long int rowCount = 0;
    map<pair<int, int>, long long> edge_weight;

    std::ifstream infile(argv[1], std::ios::app);
    std::ifstream patohfile(argv[2], std::ios::app);
    std::string line;


    map<int, vector<vector<long>> * > sub_cluster;
    map<vector<long>, float> score_tracker;
    long global_idx = 0;
    while (std::getline(patohfile, line)){   
        std::istringstream iss(line);
        string single_item;
        while (iss >> single_item){
            if(sub_cluster.find(stoi(single_item)) == sub_cluster.end()){
                sub_cluster[stoi(single_item)] = new vector<vector<long>>();
            }
            vector<long> tmp;
            tmp.push_back(global_idx);
            sub_cluster[stoi(single_item)]->push_back(tmp);
            score_tracker[tmp] = 0;
            global_idx++;
        }
        // file_line++;
    }

    // for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
    //     std::cout << it->first << ":";
    //     for(int i = 0; i < it->second->size(); i++){
    //         for(int j = 0; j < (*(it->second))[i].size(); j++)
    //             cout << (*(it->second))[i][j] << " ";
    //         cout << " : ";
    //     }
    //     cout << endl;
    // }
    // return 0;

    std::map<long int, std::vector<long int>> user_item;
    std::vector<std::vector<long int>> sorted_user_item;
    // std::map<long int, long int> obj_freq;
    long int dict_size = -1;
    long int total_access = 0;
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

        // total_access++;
        
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


    for (auto it = user_item.cbegin(); it != user_item.cend(); ++it) {
        std::vector<long int> myvector (std::begin(it->second), std::end(it->second));        
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        sorted_user_item.push_back(myvector);
    }
    // std::random_shuffle ( sorted_user_item.begin(), sorted_user_item.end() );
    user_item.clear();



    vector<int> kept_idx; 
    for (int batch_idx = 0; batch_idx < sorted_user_item.size() / 1 / 2; batch_idx++){
        for(int k = 0; k < 1; k++){
            int idx = batch_idx * 1 + k;
            // double r = ((double) rand() / (RAND_MAX));
            // if(r > ((double) SAMPLING_PERCENT / 100.0) ){
            //     continue;
            // }
            kept_idx.push_back(idx);
            total_access += sorted_user_item[idx].size();
        }
    }

    map<vector<long>, vector<long> > inverted_table;
    
    // cout << "1 " << dict_size << " " << kept_idx.size() << " " << total_access << " 2" << endl;
    for (int kk = 0; kk < kept_idx.size(); kk++){
        int idx = kept_idx[kk];
        for (int i = 0; i < sorted_user_item[idx].size(); i++){
            vector<long> tmp;
            tmp.push_back(sorted_user_item[idx][i]);
            if(inverted_table.find(tmp) == inverted_table.end()){
                inverted_table[tmp] = vector<long>();
            }
            inverted_table[tmp].push_back(idx);
            // cout << "Key insert: " << tmp[0] << endl;
        }
    }

    // for(auto it = inverted_table.cbegin(); it != inverted_table.cend(); ++it)
    // {
    //     cout << "Item (";
    //     for(int i = 0; i < it->first.size(); i++){
    //         cout << it->first[i] << " ";
    //     }
    //     cout << "): ";
    //     for(int i = 0; i < it->second.size(); i++){
    //         cout << it->second[i] << " ";
    //     }
    //     cout << endl;
    // }
    long total_capacity = stof(argv[3]) * dict_size;
    long used_capacity = 0;
    float min_ratio = 1.0 * INITIAL_MIN_RATIO;
    float min_ratio_decrement = min_ratio / 5;

    map<int, bool> keep_trying;
    for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
        keep_trying[it->first] = true;
    }

    vector<vector<long>> proposed_set_1;
    vector<vector<long>> proposed_set_2;
    vector<float> score_vec;
    for(int i = 0; i < int(sub_cluster.size()); i++){
        proposed_set_1.push_back(vector<long>());
        proposed_set_2.push_back(vector<long>());
        score_vec.push_back(0);
    }

    Timer cluster_timer;
    cluster_timer.Start();
    long iteration_cnt = 0;
    while(used_capacity < total_capacity){
        bool exit = false;
        long backup_capacity = 0;
        #pragma omp critical
        {
            // cout << "iteration cnt: " << iteration_cnt << endl;
            iteration_cnt++;
            // cout << "Trying for min_ratio_decrement: " << min_ratio_decrement << endl;
            backup_capacity = used_capacity;
            // map<long, vector<vector<long>> > backup_sub_cluster = sub_cluster;

            bool break_condition = true;
            for(auto it = keep_trying.cbegin(); it != keep_trying.cend(); ++it){
                if(it->second == true){
                    break_condition = false;
                    break;
                }
            }
            if(break_condition){
                if(min_ratio_decrement < 0.01){
                    // break;
                    exit = true;
                }
                else{
                    min_ratio -= min_ratio_decrement;
                    for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
                        keep_trying[it->first] = true;
                    }
                    if(min_ratio <= 0.0){
                        min_ratio_decrement /= 2;
                        min_ratio += min_ratio_decrement;
                    }
                }
                // cout << "Update min_ratio_decrement to: " << min_ratio_decrement << endl;
            }
        }
        if(exit){
            break;
        }

        // cout << "Finishing phase 0 of iteration " << iteration_cnt << endl;
		#pragma omp parallel for
        for(int sub_cluster_idx = 0; sub_cluster_idx < int(sub_cluster.size()); sub_cluster_idx++){
        // for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
            // long thread_id = sub_cluster_idx;
            proposed_set_1[sub_cluster_idx].clear();
            proposed_set_2[sub_cluster_idx].clear();
            score_vec[sub_cluster_idx] = 0;
            // long thread_id = it->first;
            if(keep_trying[sub_cluster_idx] == false){
                continue;
            }
            long proposed_cap = 0;
            float score = 0.0f;
            for(int j = 0; j < int(sub_cluster[sub_cluster_idx]->size()); j++){
                // assert(inverted_table.find((*(sub_cluster[sub_cluster_idx]))[j]) != inverted_table.end());
                if(inverted_table.find((*(sub_cluster[sub_cluster_idx]))[j]) == inverted_table.end()){
                    continue;
                }
                vector<long> set1 = inverted_table[(*(sub_cluster[sub_cluster_idx]))[j]];
                for(int k = j + 1; k < int(sub_cluster[sub_cluster_idx]->size()); k++){
                    // assert(inverted_table.find((*(sub_cluster[sub_cluster_idx]))[k]) != inverted_table.end());
                    if(inverted_table.find((*(sub_cluster[sub_cluster_idx]))[k]) == inverted_table.end()){
                        continue;
                    }
                    vector<long> set2 = inverted_table[(*(sub_cluster[sub_cluster_idx]))[k]];

                    std::vector<long> intersection;
                    std::set_intersection(set1.begin(),set1.end(), set2.begin(),set2.end(),
                            back_inserter(intersection));
                    
                    // cout << "Set 1: ";
                    // for(int i = 0; i < set1.size(); i++){
                    //     cout << set1[i] << " ";
                    // }
                    // cout << endl;
                    // cout << "Set 2: ";
                    // for(int i = 0; i < set2.size(); i++){
                    //     cout << set2[i] << " ";
                    // }
                    // cout << endl;
                    // cout << "Inter: ";
                    // for(int i = 0; i < intersection.size(); i++){
                    //     cout << intersection[i] << " ";
                    // }
                    // cout << endl;
                    float new_score = 1.0 * intersection.size();
                    new_score /= (pow(2, (*(sub_cluster[sub_cluster_idx]))[j].size() + (*(sub_cluster[sub_cluster_idx]))[k].size()) - pow(2, (*(sub_cluster[sub_cluster_idx]))[j].size()) - pow(2, (*(sub_cluster[sub_cluster_idx]))[k].size()) + 1);
                    
                    // cout << "Score: " << new_score; 
                    // cout << endl;
                    if(new_score > score && new_score > min_ratio){
                        score = new_score;
                        proposed_set_1[sub_cluster_idx] = ((*(sub_cluster[sub_cluster_idx]))[j]);
                        proposed_set_2[sub_cluster_idx] = ((*(sub_cluster[sub_cluster_idx]))[k]);
                        score_vec[sub_cluster_idx] = score;
                        float new_tmp = pow(2, (*(sub_cluster[sub_cluster_idx]))[j].size() + (*(sub_cluster[sub_cluster_idx]))[k].size()) - 1;
                        float old_tmp = ((*(sub_cluster[sub_cluster_idx]))[j].size() == 1) ? 0 : pow(2, (*(sub_cluster[sub_cluster_idx]))[j].size()) - 1;
                        old_tmp += ((*(sub_cluster[sub_cluster_idx]))[k].size() == 1) ? 0 : pow(2, (*(sub_cluster[sub_cluster_idx]))[k].size()) - 1;
                        proposed_cap = (new_tmp - old_tmp);
                    }
                }
            }
            #pragma omp critical
            {
                used_capacity += proposed_cap;
            }
        }
        // cout << "Finishing phase 1 of iteration " << iteration_cnt << endl;

        bool cont = false;
        
        #pragma omp critical
        {
            if(used_capacity >= total_capacity){
                for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
                    keep_trying[it->first] = true;
                }
                min_ratio += min_ratio_decrement;
                min_ratio_decrement /= 2;
                used_capacity = backup_capacity;
                // break;
                cont = true;
            }
        }

        if(cont){
            continue;
        }

        // cout << "Proposed used capcity is: " << used_capacity << " compared to total: " << total_capacity << endl;
        // cout << "Finishing phase 2 of iteration " << iteration_cnt << endl;

        #pragma omp parallel for schedule(dynamic, 1)
        for(int sub_cluster_idx = 0; sub_cluster_idx < int(sub_cluster.size()); sub_cluster_idx++){
            //do union and sort
            if(proposed_set_1[sub_cluster_idx].size() == 0 || proposed_set_2[sub_cluster_idx].size() == 0 ){
                // do something
                keep_trying[sub_cluster_idx] = false;
                continue;
            }

            // cout << "Max score : " << score << endl;
            #pragma omp critical
            {
                vector<long> item_union;
                        std::set_union(proposed_set_1[sub_cluster_idx].begin(),proposed_set_1[sub_cluster_idx].end(), proposed_set_2[sub_cluster_idx].begin(),proposed_set_2[sub_cluster_idx].end(),
                                back_inserter(item_union));
                sub_cluster[sub_cluster_idx]->erase(std::find(sub_cluster[sub_cluster_idx]->begin(), sub_cluster[sub_cluster_idx]->end(), proposed_set_1[sub_cluster_idx]));
                sub_cluster[sub_cluster_idx]->erase(std::find(sub_cluster[sub_cluster_idx]->begin(), sub_cluster[sub_cluster_idx]->end(), proposed_set_2[sub_cluster_idx]));
                sub_cluster[sub_cluster_idx]->push_back(item_union);
                score_tracker.erase(proposed_set_1[sub_cluster_idx]);
                score_tracker.erase(proposed_set_2[sub_cluster_idx]);
                score_tracker[item_union] = score_vec[sub_cluster_idx];
                vector<long> index_union;
                        std::set_union(inverted_table[proposed_set_1[sub_cluster_idx]].begin(),inverted_table[proposed_set_1[sub_cluster_idx]].end(), inverted_table[proposed_set_2[sub_cluster_idx]].begin(),inverted_table[proposed_set_2[sub_cluster_idx]].end(),
                                back_inserter(index_union));
                inverted_table.erase(proposed_set_1[sub_cluster_idx]);
                inverted_table.erase(proposed_set_2[sub_cluster_idx]);
                inverted_table[item_union] = index_union;
            
                // cout << sub_cluster_idx << " removing: ";
                // cout << "(";
                // for(int j = 0; j < proposed_set_1[sub_cluster_idx].size(); j++){
                //     cout << proposed_set_1[sub_cluster_idx][j] << ",";
                // }
                // cout << "), ";
                // cout << endl;
                // cout << sub_cluster_idx << " removing: ";
                // cout << "(";
                // for(int j = 0; j < proposed_set_2[sub_cluster_idx].size(); j++){
                //     cout << proposed_set_2[sub_cluster_idx][j] << ",";
                // }
                // cout << "), ";
                // cout << endl;
                // cout << sub_cluster_idx << " adding: ";
                // cout << "(";
                // for(int j = 0; j < item_union.size(); j++){
                //     cout << item_union[j] << ",";
                // }
                // cout << "), ";
                // cout << endl;
                
                // cout << " end of this trial\n";
            }
        }
        // cout << "Finishing phase 3 of iteration " << iteration_cnt << endl;
    }
    cluster_timer.Stop();
    // cout << "Cluster takes " << cluster_timer.Seconds() << " s. " << endl;
    // cout << "Print results <<<< " << endl;
    float total_occupied = 0;
    
    long tot_sz = 0;
    #pragma omp critical
    {
        for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
            tot_sz += it->second->size();
        }
        assert(tot_sz == score_tracker.size());

    
    // for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
    //     // cout << it->first << " : " << endl;
    //     for(int i = 0; i < it->second->size(); i++){
    //         if((*(it->second))[i].size() > 1){
    //             total_occupied += (pow(2, (*(it->second))[i].size()) - 1);
    //             // cout << "(";
    //             for(int j = 0; j < (*(it->second))[i].size(); j++){
    //                 cout << (*(it->second))[i][j] << " "; 
    //             }
    //             // cout << "), ";
    //             cout << endl;
    //         }
    //     }
    //     // cout << endl;
    // }
    // cout << "<------>" << endl;
        vector<pair<vector<long>, float> > A;
        for (auto& it : score_tracker) {
            A.push_back(it);
        }
        sort(A.begin(), A.end(), float_dict_cmp);
        for(auto it = A.cbegin(); it != A.cend(); ++it){
            if((it->first).size() <= 1){
                continue;
            }
            for(int i = 0; i < (it->first).size(); i++){
                cout << ((it->first))[i] << " "; 
                    // cout << "), ";
            }
            // cout << " : " << it->second;
            cout << endl;
        }
    }

    // #pragma omp critical
    // {
    //     for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
    //         // cout << it->first << " : " << endl;
    //         for(int i = 0; i < it->second->size(); i++){
    //             if((*(it->second))[i].size() > 1){
    //                 total_occupied += (pow(2, (*(it->second))[i].size()) - 1);
    //                 // cout << "(";
    //                 for(int j = 0; j < (*(it->second))[i].size(); j++){
    //                     cout << (*(it->second))[i][j] << " "; 
    //                 }
    //                 // cout << "), ";
    //                 cout << endl;
    //             }
    //         }
    //         // cout << endl;
    //     }
    //     // sub_cluster.clear();
    //     // cout << "total occupied: " << total_occupied << endl;
    // }

    
    // for(auto it = sub_cluster.cbegin(); it != sub_cluster.cend(); ++it){
    //     cout << it->first << " : ";
    //     for(int i = 0; i < it->second.size(); i++){
    //         if(it->second[i].size() > 1){
    //             cout << "(";
    //             for(int j = 0; j < it->second[i].size(); j++){
    //                 cout << it->second[i][j] << ",";
    //             }
    //             cout << "), ";
    //         }
    //     }
    //     cout << endl;
    // }




    
    // vector<pair<pair<int, int>, long long> > edge_weight_vec;
    // for(auto &it : edge_weight){
    //     edge_weight_vec.push_back(it);
    // }
    // sort(edge_weight_vec.begin(), edge_weight_vec.end(), edge_cmp);
    
    // // std::cout << rowCount << std::endl;
    // // std::cout << (long long)(edge_weight_vec.size() * 0.1) << std::endl;
    // for (size_t i = 0; i < edge_weight_vec.size() * 0.1; i++){
    //     std::cout << edge_weight_vec[i].first.first << " " 
    //             << edge_weight_vec[i].first.second << " " 
    //             << edge_weight_vec[i].second << "\n";
    // }

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