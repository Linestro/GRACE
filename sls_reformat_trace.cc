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

#define DIMENTION 1024
#define USER_BATCH_SIZE 64

#define OPENMP_THREAD USER_BATCH_SIZE

using namespace std;

bool long_int_cmp (long int i, long int j) { return (i<j); }
bool vector_cmp(vector<long>& a, vector<long>& b){
    return a.size() > b.size();
}
map<long, long> save_detail;
vector<long> cluster_starting_addr; 

int main( int argc, char* argv[] )
{
	omp_set_num_threads(OPENMP_THREAD);
    // std::ifstream hbm_log(argv[1], std::ios::app);
    // std::ifstream ddr_log(argv[2], std::ios::app);
    // std::ifstream hbm_graphfile(argv[1], std::ios::app);
    // std::ifstream ddr_graphfile(argv[2], std::ios::app);
    std::ifstream user_trace(argv[1], std::ios::app);
    // float hbm_cap = stof(argv[4]);
    // cout << "HBM cap ratio is: " << hbm_cap << endl;
    // assert(hbm_cap > 0.0);

    std::map<long int, std::vector<long int>> user_item;
    std::vector<std::vector<long int>> sorted_user_item;
    std::map<long int, long int> obj_freq;
    long int dict_size = -1;
    long int total_access = 0;
    std::string line;
    while (std::getline(user_trace, line))
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

    // cout << "dict size is: " << dict_size << endl;
    // cout << "User #: " << user_item.size() << endl;
    // cout << "Item #: " << obj_freq.size() << endl;
    // cout << "Access #: " << total_access << endl;
    // cout << "Avg user accesses #: " << total_access * 1.0 / user_item.size() << endl;
    // cout << "Avg obj being accessed #: " << total_access * 1.0 / obj_freq.size() << endl;
    
    long max_reduction_size = -1;
    for (auto it = user_item.cbegin(); it != user_item.cend(); ++it) {
        if(long((it->second.size()) + 1) > max_reduction_size){
            max_reduction_size = long((it->second).size() + 1);
        }
        std::vector<long int> myvector (std::begin(it->second), std::end(it->second));
        std::sort((myvector).begin(), (myvector).end(), long_int_cmp);
        sorted_user_item.push_back(myvector);
    }
    // user_item.clear();


    std::ofstream write_trace(argv[2]);
    write_trace << user_item.size() << " " << dict_size << " " << max_reduction_size << " " << endl;
    for(long i = sorted_user_item.size() / 2; i < sorted_user_item.size(); i++){
        // double r = ((double) rand() / (RAND_MAX));
        // // std::cout << "Random seed is: " << r << std::endl;
        // if(50 < 100 && r <= ((double) 50 / 100.0) ){
        //     continue;
        // }
        for(int j = 0; j < sorted_user_item[i].size(); j++){
            write_trace << sorted_user_item[i][j] << " ";
        }
        write_trace << endl;
    }
    
    user_trace.close();
    write_trace.close();
    return 0;
}