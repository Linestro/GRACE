set -Eeuo pipefail
set -x

workload=$1
execute_batch_num=${2:-16384}  # If not specified Simualte to the end of the dataset
batch_size=64
extra_flag=${@:3}
if [ -z "$extra_flag" ]
then
  extra_flag="-fopenmp"
fi

# if [[ $execute_batch_num == "" ]]; then
#   execute_batch_num=16384   # Simualte to the end of the dataset
# fi

if [[ $workload == "M1" ]]; then
  post_data_path="./dataset/merged/merged_twitch_movie_anime_steam.csv"
  aux_data_path="./dataset/merged/merged_twitch_movie_anime_steam.aux"
elif [[ $workload == "M2" ]]; then
  post_data_path="./dataset/merged/merged_clothes_office_dblp_anime.csv"
  aux_data_path="./dataset/merged/merged_clothes_office_dblp_anime.aux"
elif [[ $workload == "M3" ]]; then
  post_data_path="./dataset/merged/merged_sports_office_dblp_twitch.csv"
  aux_data_path="./dataset/merged/merged_sports_office_dblp_twitch.aux"
elif [[ $workload == "M4" ]]; then
  post_data_path="./dataset/merged/merged_sports_clothes_office_dblp.csv"
  aux_data_path="./dataset/merged/merged_sports_clothes_office_dblp.aux"
else
    echo "invalid workload given in argv 1"
    exit 0
fi


build_merge(){
  percent=${1}
  
  g++ -std=c++11 -O3 -funroll-loops -fopenmp -DSAMPLING_PERCENT=${percent} sls_graph_build_merged.cc -o bin/sls_graph_${workload}_build_${percent}_merged
  
  ./bin/sls_graph_${workload}_build_${percent}_merged ${post_data_path} ${aux_data_path} > graph/${workload}_${percent}_percent_sampling_graph.wel

}

ps=(1 2 5 10 20 50 100)
ps=(50 100)
ps=(50)
for p in "${ps[@]}"
do
  build_merge ${p} &
  # echo ${p}
done


wait
echo "${workload} processes done!"
