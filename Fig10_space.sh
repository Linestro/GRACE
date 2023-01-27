set -e
set -x


set -Eeuo pipefail
set -e
set -x



workloads=("steam" "anime" "movie" "dblp" "office" "twitch" "sports" "clothes" "M1" "M2" "M3" "M4")
# workloads=("anime")

# inference
for workload in "${workloads[@]}"
do
./run_space_hbm_only.sh ${workload} &
done
wait



echo "Fig. 10 SPACE finished"




