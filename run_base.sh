set -Eeuo pipefail
set -x

workload=$1
parent_dir=$2
execute_batch_num=${3:-16384}  # If not specified Simualte to the end of the dataset
batch_size=64
extra_flag=${@:4}
if [ -z "$extra_flag" ]
then
  extra_flag="-fopenmp"
fi

# if [[ $execute_batch_num == "" ]]; then
#   execute_batch_num=16384   # Simualte to the end of the dataset
# fi

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


run_base(){
  write_dir=${parent_dir}_${workload}_base
  mkdir -p ${write_dir}/
  store_directory=${write_dir}/
  mkdir -p ${store_directory}
  g++ -std=c++11 -O3 -funroll-loops -fopenmp -DBATCH_SIZE=${batch_size} sls_graph_base.cc -o bin/sls_graph_${workload}_base
  ./bin/sls_graph_${workload}_base ${post_data_path} > ${store_directory}/a_sls_graph_${workload}_base.log
}

run_base &

wait
echo "All processes done!"
