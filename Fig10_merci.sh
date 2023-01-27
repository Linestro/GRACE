set -e
set -x


set -Eeuo pipefail
set -e
set -x


workloads=("steam" "anime" "movie" "dblp" "office" "twitch" "sports" "clothes" "M1" "M2" "M3" "M4")
# workloads=("anime")

mkdir -p ./MERCI/graph
mkdir -p ./MERCI/cluster

# cluster
for workload in "${workloads[@]}"
do
./prepare_pbuild_one_workload.sh ${workload} &
done
wait

# inference
for workload in "${workloads[@]}"
do
./run_merci_hbm_only.sh ${workload} &
done
wait



echo "Fig. 10 MERCI finished"




