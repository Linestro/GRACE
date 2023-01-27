set -Eeuo pipefail
set -x








workloads=("anime" "steam" "movie" "alibaba" "twitch" "clothes" "flickr")
workloads=("M1" "M2" "M3" "M4")
# workloads=("m2")

for workload in "${workloads[@]}"
do
./run_build_graph_merged.sh ${workload} &
done

wait
echo "Build merged done!"
# mail -s "Script has finished" yehaojie@umich.edu <<< "Script has finished"
