set -Eeuo pipefail
set -x



mkdir -p post_trace/

workloads=("anime" "steam" "movie" "clothes" "twitch" "sports" "office" "dblp" "M1" "M2" "M3" "M4")

for workload in "${workloads[@]}"
do
  ./run_single_reformat.sh ${workload} &
done

wait
echo "All processes done!"
