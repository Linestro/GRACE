set -Eeuo pipefail
set -x







mkdir -p graph/
./run_build_graph_group.sh &
./run_build_graph_merged_group.sh &

wait

echo "Done build ICG for all datasets"