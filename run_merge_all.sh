set -Eeuo pipefail
set -x

mkdir -p dataset/merged/
./run_merge_graph.sh twitch movie anime steam &
./run_merge_graph.sh clothes office dblp anime &
./run_merge_graph.sh sports office dblp twitch &
./run_merge_graph.sh sports clothes office dblp &

wait
echo "Merge done"
