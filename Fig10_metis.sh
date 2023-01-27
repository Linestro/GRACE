set -e
set -x

prepare_metis(){
    workload=$1
    hbm_caps=("1.0")
    ## METIS
    export METIS_DLL=/usr/local/lib/libmetis.so
    line=$(ls -l | sed -n 3p result/base/base_result_${workload}_base/a_sls_graph_${workload}_base.log | sed 's/[^0-9]*//g')
    cd metis/metis-5.1.0;
    how_many_clusters=$((${line} / 128))
    start_time=$(date +%s.%3N)
    for hbm_cap in "${hbm_caps[@]}"
    do
    ./run_group.sh ${workload} ${how_many_clusters} ${hbm_cap} &
    done
    end_time=$(date +%s.%3N)
    elapsed=$(echo "scale=3; $end_time - $start_time" | bc)
    echo "METIS partition time: $elapsed"
    wait
    cd -;
}


workloads=("steam" "anime" "movie" "dblp" "office" "twitch" "sports" "clothes" "M1" "M2" "M3" "M4")
# workloads=("steam" "anime" "movie" "dblp" "office" "twitch")

## Metis clustering
for workload in "${workloads[@]}"
do
prepare_metis ${workload} &
done
wait

## Metis inference
for workload in "${workloads[@]}"
do
./run_metis_hbm_only.sh ${workload} &
done
wait

echo "Fig. 10 Metis finished"




