set -Eeuo pipefail
set -x







workloads=("anime" "steam" "movie" "sports" "twitch" "clothes" "dblp" "office")
# workloads=()
for workload in "${workloads[@]}"
do
./run_build_graph.sh ${workload} 
done

wait
echo "Build single done!"
