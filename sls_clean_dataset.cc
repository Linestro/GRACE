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
#include <omp.h>
#include <functional>
#include <limits.h>
#include <set>


using namespace std;

bool long_int_cmp (long int i, long int j) { return (i<j); }

int main(int argc, char** argv)
{
    long int rowCount = 0;
    map<pair<int, int>, long long> edge_weight;

    std::ifstream infile(argv[1], std::ios::app);
    std::ifstream graphfile(argv[2], std::ios::app);
    std::string line;

    std::map<long int, std::vector<long int>> user_item;
    std::vector<std::vector<long int>> sorted_user_item;
    std::map<long int, long int> obj_freq;
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

        total_access++;
        
        if (user_item.find(usr) == user_item.end()){
            user_item[usr] = std::vector<long int>();
        }
        user_item[usr].push_back(obj);

        
        if (obj_freq.find(obj) == obj_freq.end()){
            obj_freq[obj] = 0;
        }
        obj_freq[obj]++;

        if (obj + 1 > dict_size){
            dict_size = obj + 1;
        }
    }

    vector<long> removed_item;
    // cout << dict_size << endl;
    removed_item.resize(dict_size, 0);
    map<long, long> remain_item_map;
    long new_item_id = 0;

    for(long i = 0; i < obj_freq.size(); i++){
        if(obj_freq[i] < 10){
            removed_item[i] = 1;
        }
        else{
            if (remain_item_map.find(i) == remain_item_map.end()){
                remain_item_map[i] = new_item_id;
                new_item_id++;
            }
        }
    }

    
    for (auto it = user_item.cbegin(); it != user_item.cend(); ++it) {
        std::vector<long int> myvector (std::begin(it->second), std::end(it->second));        
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        vector<long> new_vec;
        for(int i = 0; i < myvector.size(); i++){
            if(removed_item[myvector[i]] == 0){
                new_vec.push_back(remain_item_map[myvector[i]]);
            }
        }
        if(new_vec.size() > 0){
            sorted_user_item.push_back(new_vec);
        }
    }

    std::random_shuffle(sorted_user_item.begin(), sorted_user_item.end());

    for(int i = 0; i < sorted_user_item.size(); i++){
        for(int j = 0; j < sorted_user_item[i].size(); j++){
            cout << i << "," << sorted_user_item[i][j] << endl;
        }
    }
    
    return 0;

}