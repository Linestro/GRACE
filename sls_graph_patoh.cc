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

int main(int argc, char** argv)
{
    long int rowCount = 0;
    map<pair<int, int>, long long> edge_weight;

    std::ifstream infile(argv[1], std::ios::app);
    std::ifstream graphfile(argv[2], std::ios::app);
    std::string line;

    omp_set_num_threads(NUM_OF_TABLE);

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

    rowCount = dict_size;


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

    cout << "1 " << dict_size << " " << kept_idx.size() << " " << total_access << " 2" << endl;
    for (int kk = 0; kk < kept_idx.size(); kk++){
        int idx = kept_idx[kk];
        cout << "1 ";
        
        for (int i = 0; i < sorted_user_item[idx].size(); i++){
            cout << sorted_user_item[idx][i] + 1 << " ";
        }
        cout << endl;
    }

    
    return 0;

}