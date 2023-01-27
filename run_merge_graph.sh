set -Eeuo pipefail
set -x

translate(){
  workload=$1
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

}

mix_dataset(){
  w1=$1
  w2=$2
  w3=$3
  w4=$4
  echo $w1 $w2 $w3 $w4
  translate $w1
  data1=$post_data_path
  translate $w2
  data2=$post_data_path
  translate $w3
  data3=$post_data_path
  translate $w4
  data4=$post_data_path
  
  echo $data1 $data2 $data3 $data4
}

g++ -std=c++11 -O3 -funroll-loops -fopenmp sls_merge_graph.cc -o bin/sls_merge_graph_$1_$2_$3_$4

mix_dataset $1 $2 $3 $4

./bin/sls_merge_graph_$1_$2_$3_$4 $data1 $data2 $data3 $data4 dataset/merged/merged_$1_$2_$3_$4.aux > dataset/merged/merged_$1_$2_$3_$4.csv


wait

echo "Merge $1 $2 $3 $4 done!"
