// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#include <cinttypes>
#include <limits>
#include <iostream>
#include <queue>
#include <vector>

#include "benchmark.h"
#include "builder.h"
#include "command_line.h"
#include "graph.h"
#include "platform_atomics.h"
#include "pvector.h"
#include "timer.h"
#include <string>
#include <algorithm>
#include <map>
#include <cassert>
#include <cmath>
#include <set>
#include <fstream>
#include <omp.h>
#define NUM_THREADS 240

// #define hbm_cap_ratio 1.0

using namespace std;

std::map<int, long> connectivity_map;
std::vector<int> connectivity_rank;

vector<vector<int> > cluster_vector;

double lower_bound_sum = 0.0;
double upper_bound_sum = 0.0;

long occupied_space = 0;
long maximum_space = 0;

float alpha = 0.50;
int max_length = 0;

bool cmp(pair<int, long>& a,
         pair<int, long>& b)
{
    return a.second > b.second;
}

int edge_weight(WGraph &g, int a, int b){
	for (NodeWeight<int, int> u1 : g.out_neigh(a)) {
		if(u1.v == b)
			return u1.w;
	}
	return 0;
}

struct score_bound{
	double lower_bound = 0.0;
	double upper_bound = 0.0;
	double score = 0.0;
};

struct decay_length_hbm_cap{
	int decay;
	int length;
	float hbm_cap;
};

void view_clusters(ofstream &hbm_cluster, ofstream &ddr_cluster, long hbm_line){
	// cout << "---\n";
	long perspective = 0;
	for(size_t i = 0; i < cluster_vector.size(); i++){
		perspective += (pow(2, cluster_vector[i].size()) - 1);
		if(perspective <= hbm_line){
			for(size_t j = 0; j < cluster_vector[i].size(); j++){
				hbm_cluster << cluster_vector[i][j] << " ";
			}
			hbm_cluster << endl;
		}
		else{
			for(size_t j = 0; j < cluster_vector[i].size(); j++){
				ddr_cluster << cluster_vector[i][j] << " ";
			}
			ddr_cluster << endl;
		}
		// cout << "( ";
		// cout << "); ";
		// cout << endl;
	}
	// cout << "\n---\n";
}

int cached_space_check_on_insert(int insert_cluster_id){
	int ret = 0;
	for(size_t i = 0; i < cluster_vector.size(); i++){
		if(int(i) == insert_cluster_id)
			ret += (pow(2.0, cluster_vector[i].size() + 1) - 1);
		else
			ret += (pow(2.0, cluster_vector[i].size()) - 1);
	}
	return ret;
}

score_bound return_score(WGraph &g, int node, vector<int> cluster, int cluster_id){
	score_bound ret;
	double new_score = 0;
	vector<int> e_weight;
	for(size_t i = 0; i < cluster.size(); i++){
		for(size_t j = i + 1; j < cluster.size(); j++){
			e_weight.push_back(edge_weight(g, cluster[i], cluster[j]));
		}
	}
	for(size_t i = 0; i < cluster.size(); i++){
		e_weight.push_back(edge_weight(g, cluster[i], node));
	}

	sort(e_weight.begin(), e_weight.end());

	for(size_t i = 0; i < e_weight.size(); i++){
		if(i < cluster.size() || i == 1){
			new_score += e_weight[i];
			ret.lower_bound = new_score;
			ret.upper_bound = new_score;
		}
		else{
			new_score += e_weight[i];
			ret.upper_bound = new_score;
		}
	}

	// float alpha = 0.50;
	ret.score = (1 - alpha) * ret.lower_bound + alpha * ret.upper_bound;

	ret.score /= (1.0 * (pow(2.0, cluster.size() + 1) - 1) - (cluster.size() + 1));

	return ret;
}

void insert_cluster_id(WGraph &g, int node, vector<bool> &visited, int n, double decay, ofstream &cluster_score){
	visited[node] = true;
	set<int> candidate;
	cluster_vector.push_back(vector<int>());
	// cout << "Starting with pivot node ID: " << node << endl;
	int ret_insert_cluster_id = cluster_vector.size() - 1;

	cluster_vector[ret_insert_cluster_id].push_back(node);
	
	// for (NodeWeight<int, int> u1 : g.out_neigh(u)) {
	// 	tmp += u1.w;
	// }

	for (NodeWeight<int, int> u1 : g.out_neigh(node)) {
		if(visited[u1.v] == false){
			candidate.insert(u1.v);
		}
	}

	if(candidate.size() == 0){
		// cluster_vector.pop_back();
		for(int i = 0; i < int(cluster_vector[ret_insert_cluster_id].size()); i++)
			cluster_score << cluster_vector[ret_insert_cluster_id][i] << " ";
		cluster_score << endl;
		cluster_score << "Cluster ID: " << ret_insert_cluster_id << " size of " <<  cluster_vector[ret_insert_cluster_id].size() << " Lower bound: " << 0 << " Higher bound: " << 0 << " Estimated score: " << 0 << endl;
		return;
	}

	double max_score = 0.0;
	score_bound max_bound;
	while(true){
		int best_cand = -1;
		// max_score *= (decay * 1.0 * (occupied_space + (long)(pow(2.0, cluster_vector[ret_insert_cluster_id].size() + 1) - 1)) / (1.0 * maximum_space) / 100.0);
		max_score *= (decay * 1.0 / 100.0);
		// max_score = 0;
		if(((occupied_space + (long)(pow(2.0, cluster_vector[ret_insert_cluster_id].size() + 1) - 1) - (long)(cluster_vector[ret_insert_cluster_id].size() + 1)) >= maximum_space)
			|| (int(cluster_vector[ret_insert_cluster_id].size()) >= max_length)	
		)
			break;

		vector<int> candi_set(candidate.begin(), candidate.end());
		#pragma omp parallel for schedule(dynamic, NUM_THREADS)
		for (int i = 0; i < int(candi_set.size()); i++){
			score_bound this_score = return_score(g, candi_set[i], cluster_vector[ret_insert_cluster_id], ret_insert_cluster_id);
			// cout << "Trial of cluster " << i << " gives score " << this_score << endl;
			#pragma omp critical
			if(this_score.score >= max_score){
				max_score = this_score.score;
				max_bound = this_score;
				best_cand = candi_set[i];
			}
		}
		
		if(best_cand < 0){
			for(int i = 0; i < int(cluster_vector[ret_insert_cluster_id].size()); i++)
				cluster_score << cluster_vector[ret_insert_cluster_id][i] << " ";
			cluster_score << endl;
			lower_bound_sum += max_bound.lower_bound;
			upper_bound_sum += max_bound.upper_bound;
			cluster_score << "Cluster ID: " << ret_insert_cluster_id << " size of " <<  cluster_vector[ret_insert_cluster_id].size() << " Lower bound: " << max_bound.lower_bound << " Higher bound: " << max_bound.upper_bound << " Estimated score: " << max_score * ((pow(2.0, cluster_vector[ret_insert_cluster_id].size()) - 1) - cluster_vector[ret_insert_cluster_id].size()) / (decay * 1.0 / 100.0) << endl;
			return;
		}
		cluster_vector[ret_insert_cluster_id].push_back(best_cand);
		candidate.erase(best_cand);
		visited[best_cand] = true;
		
		for (NodeWeight<int, int> u1 : g.out_neigh(best_cand)) {
			if(visited[u1.v] == false){
				candidate.insert(u1.v);
			}
		}

		if(candidate.size() == 0){
			for(int i = 0; i < int(cluster_vector[ret_insert_cluster_id].size()); i++)
				cluster_score << cluster_vector[ret_insert_cluster_id][i] << " ";
			cluster_score << endl;
			lower_bound_sum += max_bound.lower_bound;
			upper_bound_sum += max_bound.upper_bound;
			cluster_score << "Cluster ID: " << ret_insert_cluster_id << " size of " <<  cluster_vector[ret_insert_cluster_id].size() << " Lower bound: " << max_bound.lower_bound << " Higher bound: " << max_bound.upper_bound << " Estimated score: " << max_score * ((pow(2.0, cluster_vector[ret_insert_cluster_id].size()) - 1) - cluster_vector[ret_insert_cluster_id].size()) / (decay * 1.0 / 100.0) << endl;
			return;
		}

		// cout << "Best cand: " << best_cand << " Score: " << max_score * 1.0 * (pow(2.0, cluster_vector[ret_insert_cluster_id].size()) - 1) << endl;
	// 	visited[best_cand] = true;
	// }
	// if(cluster_vector[ret_insert_cluster_id].size() == 1){
	// 	cluster_vector.pop_back();
	// }
		// for(int i = 0; i < int(cluster_vector[ret_insert_cluster_id].size()); i++)
		// 	cout << cluster_vector[ret_insert_cluster_id][i] << " ";
		// cout << endl;
	}
	for(int i = 0; i < int(cluster_vector[ret_insert_cluster_id].size()); i++)
		cluster_score << cluster_vector[ret_insert_cluster_id][i] << " ";
	cluster_score << endl;
	lower_bound_sum += max_bound.lower_bound;
	upper_bound_sum += max_bound.upper_bound;
	cluster_score << "Cluster ID: " << ret_insert_cluster_id << " size of " <<  cluster_vector[ret_insert_cluster_id].size() << " Lower bound: " << max_bound.lower_bound << " Higher bound: " << max_bound.upper_bound << " Estimated score: " << max_score * ((pow(2.0, cluster_vector[ret_insert_cluster_id].size()) - 1) - cluster_vector[ret_insert_cluster_id].size()) / (decay * 1.0 / 100.0) << endl;
}

int main(int argc, char* argv[]) {
	omp_set_num_threads(NUM_THREADS);
    CLDelta<WeightT> cli(argc, argv, "find cluster");
    if (!cli.ParseArgs())
        return -1;
    WeightedBuilder b(cli);
    WGraph g = b.MakeGraph();
	long degree_tot = 0;
	for(NodeID u = 0; u < g.num_nodes(); u++){
		for (NodeWeight<int, int> u1 : g.out_neigh(u)) {
            degree_tot++;
        }
	}

    for(NodeID u = 0; u < g.num_nodes(); u++){
        long tmp = 0;
        for (NodeWeight<int, int> u1 : g.out_neigh(u)) {
            tmp += u1.w;
        }
        connectivity_map[u] = tmp;
    }

	vector<pair<int, long> > connectivity_map_vec;
	for (auto& it : connectivity_map) {
		connectivity_map_vec.push_back(it);
    }
	sort(connectivity_map_vec.begin(), connectivity_map_vec.end(), cmp);
	for (auto& it : connectivity_map_vec) {
		connectivity_rank.push_back(it.first);
    }
    vector<string> hbm_cap_group;
    hbm_cap_group.push_back("1.0");
    // hbm_cap_group.push_back("0.5");
    // hbm_cap_group.push_back("0.25");

    vector<pair<string, decay_length_hbm_cap>> prefix_group;

    string workload(argv[3]);
	int decay = stoi(argv[4]);
	int length = stoi(argv[5]);
	for(int hbm_cap_idx = 0; hbm_cap_idx < int(hbm_cap_group.size()); hbm_cap_idx += 1){
		string prefix = workload + "_decay_" + to_string(decay) + "_sampling_50_alpha_50_length_" + to_string(length) + "_hbm_" + hbm_cap_group[hbm_cap_idx];
		decay_length_hbm_cap tmp;
		tmp.decay = decay;
		tmp.length = length;
		tmp.hbm_cap = stof(hbm_cap_group[hbm_cap_idx]);
		prefix_group.push_back(make_pair(prefix, tmp));
		// cout << prefix << endl;
	}

	for(int prefix_idx = 0; prefix_idx < int(prefix_group.size()); prefix_idx++){
	vector<vector<int> >().swap(cluster_vector);

	lower_bound_sum = 0.0;
	upper_bound_sum = 0.0;

	occupied_space = 0;
	maximum_space = 0;

	alpha = 0.50;
	max_length = 0;

	std::ofstream cluster_score("cluster/" + (prefix_group[prefix_idx]).first + ".log");
	std::ofstream hbm_cluster("cluster/" + (prefix_group[prefix_idx]).first + ".hbm.cluster");
	std::ofstream ddr_cluster("cluster/" + (prefix_group[prefix_idx]).first + ".ddr.cluster");
	cluster_score.close();
	hbm_cluster.close();
	ddr_cluster.close();

    cluster_score.open("cluster/" + (prefix_group[prefix_idx]).first + ".log", fstream::app);
    hbm_cluster.open("cluster/" + (prefix_group[prefix_idx]).first + ".hbm.cluster", fstream::app);
    ddr_cluster.open("cluster/" + (prefix_group[prefix_idx]).first + ".ddr.cluster", fstream::app);

	double decay = prefix_group[prefix_idx].second.decay * 1.0;
	max_length = prefix_group[prefix_idx].second.length;
	float hbm_cap_ratio = prefix_group[prefix_idx].second.hbm_cap;

	cluster_score << "HBM is: " << hbm_cap_ratio << endl;
	cluster_score << "Alpha is: " << alpha << endl;
	cluster_score << "decay is: " << decay << endl;
	cluster_score << "Max cluster length is: " << max_length << endl;

	cluster_score << "Number of node is: " << g.num_nodes() << endl;
	cluster_score << "Avg degree of node is: " << degree_tot * 1.0 / g.num_nodes() << endl;


	int insertion_node_idx = 0;
    int n = g.num_nodes();
	// maximum_space = (long) (1.1*n);
	maximum_space = (long) ((1.0 + hbm_cap_ratio)*n);
	if(maximum_space < 16){
		maximum_space = 16;
	}
	vector<bool> visited;
	visited.resize(n, false);
	occupied_space = n;

	Timer t;
	t.Start();
	while(insertion_node_idx < int(connectivity_rank.size())){
		insert_cluster_id(g, connectivity_rank[insertion_node_idx], visited, n, decay, cluster_score);
		occupied_space += ((pow(2.0, cluster_vector[cluster_vector.size() - 1].size()) - 1) - cluster_vector[cluster_vector.size() - 1].size());
		if(occupied_space >= maximum_space)
			break;
		// cluster_vector[insert_id].push_back(connectivity_rank[insertion_node_idx]);
		// view_clusters();
		while(insertion_node_idx < int(connectivity_rank.size()) && 
				visited[connectivity_rank[insertion_node_idx]] == true){
			insertion_node_idx++;
		}
	}
	t.Stop();

	view_clusters(hbm_cluster, ddr_cluster, (long)(hbm_cap_ratio*n));
	// cout << "It takes " << t.Millisecs() << " ms. " << endl;
	cluster_score << "It takes " << t.Millisecs() << " ms. " << endl;
	// cluster_score << "Lower bound prediction: " << lower_bound_sum << endl;
	// cluster_score << "Upper bound prediction: " << upper_bound_sum << endl;
	cluster_score.close();
	hbm_cluster.close();
	ddr_cluster.close();
	}
  	return 0;
}
