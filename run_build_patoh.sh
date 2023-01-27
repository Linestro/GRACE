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


if [[ $workload == "anime" ]]; then
  post_data_path="./dataset/anime/rating_postprocess.csv"
elif [[ $workload == "office" ]]; then
  post_data_path="./dataset/office/amazon_Office_Products_postprocess.csv"
elif [[ $workload == "dblp" ]]; then
  post_data_path="./dataset/dblp/dblp_postprocess.csv"
elif [[ $workload == "sports" ]]; then
  post_data_path="./dataset/sports/amazon_Sports_and_Outdoors_postprocess.csv"
elif [[ $workload == "M1" ]]; then
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
elif [[ $workload == "steam" ]]; then
  post_data_path="./dataset/steam/australian_users_items_postprocess.csv"
elif [[ $workload == "movie" ]]; then
  post_data_path="./dataset/movie/rating_postprocess.csv"
elif [[ $workload == "twitch" ]]; then
  post_data_path="./dataset/twitch/100k_a_postprocess.csv"
elif [[ $workload == "clothes" ]]; then
  post_data_path="./dataset/clothes/amazon_Clothing_Shoes_and_Jewelry_postprocess.csv"
else
    echo "invalid workload given in argv 1"
    exit 0
fi

build_patoh(){
  percent=${1}
  g++ -std=c++11 -O3 -funroll-loops -fopenmp -DSAMPLING_PERCENT=${percent} sls_graph_patoh.cc -o bin/sls_graph_${workload}_patoh_${percent}
  
  ./bin/sls_graph_${workload}_patoh_${percent} ${post_data_path} > ./MERCI/graph/${workload}_patoh.graph

}

ps=(1 2 5 10 20 50 100)
ps=(50 100)
ps=(50)
for p in "${ps[@]}"
do
  build_patoh ${p} &
  # echo ${p}
done


wait
echo "All processes done!"
