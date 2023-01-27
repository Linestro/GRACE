set -e
set -x
# nvcc -Xcompiler -fopenmp -Xptxas -O3 merci_inference.cu -o ./bin/merci.out
if [[ $# -eq 0 ]] ; then
    echo 'give me workload'
    exit 0
fi

workload=$1
# hbm_cap=$2

if [[ $workload == "anime" ]]; then
  data_path="./dataset/anime/rating_preprocess.csv"
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
  data_path="./dataset/steam/australian_users_items_preprocess.csv"
  post_data_path="./dataset/steam/australian_users_items_postprocess.csv"
elif [[ $workload == "movie" ]]; then
  data_path="./dataset/movie/rating_preprocess.csv"
  post_data_path="./dataset/movie/rating_postprocess.csv"
elif [[ $workload == "twitch" ]]; then
  data_path="./dataset/twitch/100k_a_preprocess.csv"
  post_data_path="./dataset/twitch/100k_a_postprocess.csv"
elif [[ $workload == "clothes" ]]; then
  post_data_path="./dataset/clothes/amazon_Clothing_Shoes_and_Jewelry_postprocess.csv"

elif [[ $workload == "space_anime" ]]; then
  post_data_path="./dataset/anime/rating_preprocess.csv"
elif [[ $workload == "space_office" ]]; then
  post_data_path="./dataset/office/Office_Products_preprocess.csv"
elif [[ $workload == "space_dblp" ]]; then
  post_data_path="./dataset/dblp/ca-coauthors-dblp_preprocess.csv"
elif [[ $workload == "space_sports" ]]; then
  post_data_path="./dataset/sports/Sports_and_Outdoors_preprocess.csv"
elif [[ $workload == "space_m1" ]]; then
  post_data_path="./dataset/merged/merged_space_sports_space_clothes_space_office_space_dblp.csv"
elif [[ $workload == "space_m2" ]]; then
  post_data_path="./dataset/merged/merged_space_sports_space_office_space_dblp_space_twitch.csv"
elif [[ $workload == "space_m4" ]]; then
  post_data_path="./dataset/merged/merged_space_twitch_space_movie_space_anime_space_steam.csv"
elif [[ $workload == "space_m5" ]]; then
  post_data_path="./dataset/merged/merged_space_clothes_space_office_space_dblp_space_anime.csv"
elif [[ $workload == "space_steam" ]]; then
  post_data_path="./dataset/steam/australian_users_items_preprocess.csv"
elif [[ $workload == "space_movie" ]]; then
  post_data_path="./dataset/movie/rating_preprocess.csv"
elif [[ $workload == "space_twitch" ]]; then
  post_data_path="./dataset/twitch/100k_a_preprocess.csv"
elif [[ $workload == "space_clothes" ]]; then
  post_data_path="./dataset/clothes/Clothing_Shoes_and_Jewelry_preprocess.csv"
else
    echo "invalid workload given in argv 1"
    exit 0
fi

mkdir -p hbm_only_space_log/

g++ -fopenmp -O3 hbm_only_space_inference_cnt.cc -o ./bin/space_hbm_only_cnt_${workload}.out

./bin/space_hbm_only_cnt_${workload}.out ${workload} post_trace/${workload}.trace ${post_data_path} 


wait

echo "Finished"