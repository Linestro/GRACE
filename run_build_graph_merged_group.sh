set -Eeuo pipefail
set -x








workloads=("M1" "M2" "M3" "M4")

for workload in "${workloads[@]}"
do
./run_build_graph_merged.sh ${workload} 
done

wait
echo "Build merged done!"
