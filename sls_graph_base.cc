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

int main(int argc, char** argv)
{
    long int rowCount = 0;
    map<pair<int, int>, long long> edge_weight;

    std::ifstream infile(argv[1], std::ios::app);
    // std::ifstream graphfile(argv[2], std::ios::app);
    std::string line;

    omp_set_num_threads(NUM_OF_TABLE);

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

    cout << "dict size is: " << dict_size << endl;
    cout << "User #: " << user_item.size() << endl;
    cout << "Item #: " << obj_freq.size() << endl;
    cout << "Access #: " << total_access << endl;
    cout << "Avg user accesses #: " << total_access * 1.0 / user_item.size() << endl;
    cout << "Avg obj being accessed #: " << total_access * 1.0 / obj_freq.size() << endl;

    return 0;

}