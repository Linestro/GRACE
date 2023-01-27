# set -Eeuo pipefail
set -x


workload=("$1")
hbm_caps=("1.0")

### BASE
mkdir -p result/base/
./run_base.sh ${workload} result/base/base_result &
wait


# ## PATOH
./run_build_patoh.sh ${workload} &
wait

line=$(ls -l | sed -n 3p result/base/base_result_${workload}_base/a_sls_graph_${workload}_base.log | sed 's/[^0-9]*//g')
how_many_clusters=$((${line} / 128))
cd ./MERCI;
start_time=$(date +%s.%3N)
./2_partition/bin/patoh ./graph/${workload}_patoh.graph ${how_many_clusters} BA=0
end_time=$(date +%s.%3N)
elapsed=$(echo "scale=3; $end_time - $start_time" | bc)
echo "${workload} PATOH partition time: $elapsed"
cd -;

## MERCI
line=$(ls -l | sed -n 3p result/base/base_result_${workload}_base/a_sls_graph_${workload}_base.log | sed 's/[^0-9]*//g')
how_many_clusters=$((${line} / 128))
for hbm_cap in "${hbm_caps[@]}"
do
./run_cluster_patoh.sh ${workload} ${how_many_clusters} ${hbm_cap} &
done
wait


echo "${workload} Finished"