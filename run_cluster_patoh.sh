set -Eeuo pipefail
set -x
set -e

workload=$1
how_many_clusters=$2
hbm_cap=$3
execute_batch_num=16384  # If not specified Simualte to the end of the dataset
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
elif [[ $workload == "reddit" ]]; then
  data_path="./dataset/reddit/reddit_preprocess.txt"
elif [[ $workload == "gmap" ]]; then
  data_path="./dataset/google_map/reviews_preprocess.csv"
elif [[ $workload == "tv" ]]; then
  data_path="./dataset/amazon_tv/ratings_Movies_and_TV_preprocess.csv"
elif [[ $workload == "amazon_cd" ]]; then
  data_path="./dataset/amazon_cd/ratings_CDs_and_Vinyl_preprocess.csv"
elif [[ $workload == "kindle" ]]; then
  data_path="./dataset/amazon_kindle/ratings_Kindle_Store_preprocess.csv"
elif [[ $workload == "movie" ]]; then
  data_path="./dataset/movie/rating_preprocess.csv"
  post_data_path="./dataset/movie/rating_postprocess.csv"
elif [[ $workload == "movie_1m" ]]; then
  data_path="./dataset/movie_1m/ratings_preprocess.csv"
  batch_size=1
elif [[ $workload == "fm" ]]; then
  data_path="./dataset/fm/userid-timestamp-artid-artname-traid-traname_preprocess.csv"
  batch_size=1
elif [[ $workload == "twitch" ]]; then
  data_path="./dataset/twitch/100k_a_preprocess.csv"
  post_data_path="./dataset/twitch/100k_a_postprocess.csv"
elif [[ $workload == "electric" ]]; then
  data_path="./dataset/amazon_electric/Electronics_preprocess.csv"
elif [[ $workload == "book" ]]; then
  data_path="./dataset/amazon_books/Books_preprocess.csv"
elif [[ $workload == "journal" ]]; then
  data_path="./dataset/livejournal/livejournal-groupmemberships/out_preprocess.csv"
elif [[ $workload == "yahoo" ]]; then
  data_path="./dataset/yahoo/yahoo-song/out_preprocess.csv"
elif [[ $workload == "wiki" ]]; then
  data_path="./dataset/wiki/edit-eswiki/out_preprocess.csv"
elif [[ $workload == "flickr" ]]; then
  data_path="./dataset/flickr/flickr-groupmemberships/out_preprocess.csv"
elif [[ $workload == "journal_new" ]]; then
  data_path="./dataset/livejournal_new/livejournal.edges"
elif [[ $workload == "imdb" ]]; then
  data_path="./dataset/imdb/actor2/out_preprocess.csv"
elif [[ $workload == "webtracker" ]]; then
  data_path="./dataset/webtracker/trackers-trackers/out_preprocess.csv"
elif [[ $workload == "cell" ]]; then
  data_path="./dataset/cell/Cell_Phones_and_Accessories_preprocess.csv"
# elif [[ $workload == "sports" ]]; then
#   data_path="./dataset/sports/Sports_and_Outdoors_preprocess.csv"
elif [[ $workload == "clothes" ]]; then
  # data_path="./dataset/clothes/Clothing_Shoes_and_Jewelry_preprocess.csv"
  post_data_path="./dataset/clothes/amazon_Clothing_Shoes_and_Jewelry_postprocess.csv"
elif [[ $workload == "toy" ]]; then
  data_path="./dataset/toy/toy.csv"
elif [[ $workload == "alibaba" ]]; then
  data_path="./dataset/alibaba/raw_sample_preprocess.csv"
  post_data_path="./dataset/alibaba/raw_sample_postprocess.csv"
elif [[ $workload == "criteo" ]]; then
  data_path="./dataset/criteo/ratings_preprocess.csv.0"
elif [[ $workload == "avazu" ]]; then
  data_path="./dataset/avazu/train_preprocess.csv.0"
elif [[ $workload == "anime_steam" ]]; then
  data_path="./dataset/anime_steam/final_rating.csv"
else
    echo "invalid workload given in argv 1"
    exit 0
fi

merci_cluster(){
  # shape=$1
  percent=$1
  hbm_cap=$2
  g++ -std=c++11 -g -O3 -funroll-loops -fopenmp -DSAMPLING_PERCENT=${percent} sls_cluster_patoh.cc -o bin/sls_cluster_${workload}_patoh_${percent}_hbm_${hbm_cap}
  
  start_time=$(date +%s.%3N)
  ./bin/sls_cluster_${workload}_patoh_${percent}_hbm_${hbm_cap} ${post_data_path} ./MERCI/graph/${workload}_patoh.graph.part.${how_many_clusters} ${hbm_cap} > ./MERCI/cluster/${workload}_patoh_hbm_${hbm_cap}.cluster
  end_time=$(date +%s.%3N)
  elapsed=$(echo "scale=3; $end_time - $start_time" | bc)
  echo "${workload} MERCI cluster time: $elapsed"
}

ps=(1 2 5 10 20 50 100)
ps=(50 100)
ps=(50)
for p in "${ps[@]}"
do
  merci_cluster ${p} ${hbm_cap} &
done


wait
echo "${workload} processes done!"
