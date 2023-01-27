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
#include <cmath>

#define NUM_OF_TABLE 1
// #define BATCH_SIZE 64
#define DIMENSION 64
// #define PRINT_TRACE

#define START_MEM 0x000000000
#define INDIV_TABLE_SIZE 0x100000000
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

#define CACHE_ITEM_RATIO 1

using namespace std;

bool long_int_cmp (long int i, long int j) { return (i<j); }
bool sw_dict_cmp(pair<long int, long int>& a, pair<long int, long int>& b){
    return a.second != b.second?  a.second > b.second : a.first > b.first;
}
bool vector_cmp(vector<int>& a, vector<int>& b){
    return a.size() > b.size();
}

vector<map<int, int> > save_detail;
vector<long> cluster_starting_addr; 

long long orig = 0;
long long actual = 0;
int curious = 0;
long long ddr_count = 0;
long hbm_count = 0;

double total_time = 0.0;
int total_time_cnt = 0;

int main(int argc, char** argv)
{
    long int rowCount = 0;
    map<pair<int, int>, long long> edge_weight;

    std::vector<std::ifstream*> infile;
    std::ifstream infile1(argv[1], std::ios::app);
    std::ifstream infile2(argv[2], std::ios::app);
    std::ifstream infile3(argv[3], std::ios::app);
    std::ifstream infile4(argv[4], std::ios::app);
    std::ofstream ofile5;
	ofile5.open(argv[5]);
	ofile5.close();
	ofile5.open(argv[5], fstream::app);

    infile.push_back(&infile1);
    infile.push_back(&infile2);
    infile.push_back(&infile3);
    infile.push_back(&infile4);
    // std::ifstream graphfile(argv[2], std::ios::app);
    // int sampling_precent = std::stoi(argv[3]);
    std::string line;

    omp_set_num_threads(NUM_OF_TABLE);

    std::map<long int, std::vector<long int>> global_user_item;
    std::vector<std::vector<long int>> sorted_global_user_item;

    long int total_access = 0;
    long offset_dict_size = 0;
    ofile5 << offset_dict_size << endl;
    
    for(int idx = 0; idx < 4; idx++){
        std::map<long int, std::vector<long int>> user_item;
        std::map<long int, long int> obj_freq;
        long int dict_size = 0;
        while (std::getline(*(infile[idx]), line))
        {   
            std::istringstream iss(line);
            long int usr = 0;
            long int obj = 0;
            char dummy;
            if (!(iss >> usr >> dummy >> obj)) { break; } // error
            obj += offset_dict_size;
            // assert(usr < 16384 * 64);
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
        offset_dict_size = dict_size;
        long user_size = user_item.size();
        for(int i = 0; i < 16384 * 64; i++){
            for(int j = 0; j < user_item[i % user_size].size(); j++){
                global_user_item[i].push_back(user_item[i % user_size][j]);
            }
        }
        user_item.clear();
        obj_freq.clear();

        ofile5 << offset_dict_size << endl;
    }



    // std::cout << "dict size is: " << offset_dict_size << endl;
    // std::cout << "User #: " << global_user_item.size() << endl;
    // // std::cout << "Item #: " << obj_freq.size() << endl;
    // std::cout << "Access #: " << total_access << endl;
    // std::cout << "Avg user accesses #: " << total_access * 1.0 / global_user_item.size() << endl;
    // std::cout << "Avg obj being accessed #: " << total_access * 1.0 / offset_dict_size << endl;

    rowCount = offset_dict_size;
    cluster_starting_addr.push_back(rowCount);
    int ddr_starting_line = 0;

    vector<vector<int> > cached_ps;
    map<int, int> item_to_cacheline;
    vector<bool> is_cached;
    // is_cached.resize(obj_freq.size(), false);



    // for(int i = 0; i < cached_ps.size(); i++){
    //     for(int j = 0; j < cached_ps[i].size(); j++){
    //         cout << cached_ps[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // cout << "Start inferencing batches..." << endl;

    for (auto it = global_user_item.cbegin(); it != global_user_item.cend(); ++it) {
        std::vector<long int> myvector (std::begin(it->second), std::end(it->second));
        
        // for(int i = 0; i < myvector.size(); i++){
        //     myvector[i] = space_reorder_ind[myvector[i]];
        // }
        
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        // sorted_global_user_item.push_back(myvector);
        for(int i = 0; i < myvector.size(); i++){
            std::cout << (it->first) << "," << myvector[i] << endl;
        }
    }
    // // std::random_shuffle ( sorted_user_item.begin(), sorted_user_item.end() );
    // user_item.clear();


    // cout << "Curious: " << curious << endl;
    
    return 0;

}