

workloads=("anime" "steam" "movie" "clothes" "twitch" "office" "dblp" "sports" "m1" "m2" "m4" "m5")

for workload in "${workloads[@]}"
do

    cp ${workload}_hbm_1.0.cluster /data3/reduction/merci_cluster/${workload}_patoh_hbm_1.0.cluster

done