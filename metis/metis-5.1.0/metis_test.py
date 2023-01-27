import numpy as np
from matplotlib import pyplot as plt
from scipy.sparse import csr_matrix, lil_matrix, diags
# from sklearn.cluster import SpectralClustering
import networkx as nx
import argparse
import math
import metis
import random
parser = argparse.ArgumentParser()

parser.add_argument('-i', required=True)
parser.add_argument('-c', required=True)
parser.add_argument('-b', required=True)

args = parser.parse_args()

in_nodes = []
out_nodes = []
weights = []
with open(args.i) as f:
    for ln in f.readlines():
        in_nodes.append(int(ln.split(' ')[0]))
        out_nodes.append(int(ln.split(' ')[1]))
        weights.append(int(ln.split(' ')[2]))
        # for symmetry
        in_nodes.append(int(ln.split(' ')[1]))
        out_nodes.append(int(ln.split(' ')[0]))
        weights.append(int(ln.split(' ')[2]))

# print(in_nodes)
# print(out_nodes)
# print(weights)

# Create a weighted adjacency sparse matrix from the data
max_nodes = max(max(in_nodes) + 1, max(out_nodes) + 1)
sparse_weighted_adj = csr_matrix((weights, (in_nodes, out_nodes)), shape=(max_nodes, max_nodes))
# sparse_weighted_adj[in_nodes, out_nodes] = weights
# sparse_weighted_adj[out_nodes, in_nodes] = weights
# print(sparse_weighted_adj)

color_offset = 0
# Remove isolated nodes as they adversely affect clustering
global_G = nx.Graph(sparse_weighted_adj)
global_G.remove_nodes_from(list(nx.isolates(global_G)))

nz_remap = {}
for i in range(len(list(global_G))):
    nz_remap[i] = list(global_G)[i]

num_clusters = int(args.c)
# G = nx.from_numpy_matrix(np.int32(A))  
# otherwise metis will not recognize you have a weighted graph
[cost, parts] = metis.part_graph(global_G, nparts=num_clusters, recursive=True)


# print(parts)

cluster = []
for i in range(num_clusters):
    cluster.append([])

occupied = 0
budget = float(args.b)*max_nodes
l = list(range(len(parts)))
random.shuffle(l)

for i in l:
    old_size = len(cluster[parts[i]])
    occupied -= ((2 ** old_size) - 1)
    occupied += ((2 ** (old_size+1)) - 1)
    if occupied >= budget:
        break
    cluster[parts[i]].append(nz_remap[i])


# print(occupied)
# print(cluster)

for i in range(len(cluster)):
    for j in range(len(cluster[i])):
        print(cluster[i][j],end=' ')
    if len(cluster[i]) > 0:
        print('')
