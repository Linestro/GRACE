set -Eeuo pipefail
set -x

# workload=$1


program="metis_test.py"
mkdir -p cluster/
workload=$1
how_many_clusters=$2
hbm_cap=$3

python ${program} -i ../../graph/${workload}_50_percent_sampling_graph.wel -c ${how_many_clusters} -b ${hbm_cap} > cluster/${workload}_hbm_${hbm_cap}.cluster 
