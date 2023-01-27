set -Eeuo pipefail
set -x

translate(){
  workload=$1
if [[ $workload == "anime" ]]; then
  data_path="./dataset/anime/rating_preprocess.csv"
  post_data_path="./dataset/anime/rating_postprocess.csv"
elif [[ $workload == "office" ]]; then
  post_data_path="./dataset/office/amazon_Office_Products_postprocess.csv"
elif [[ $workload == "dblp" ]]; then
  post_data_path="./dataset/dblp/dblp_postprocess.csv"
elif [[ $workload == "sports" ]]; then
  post_data_path="./dataset/sports/amazon_Sports_and_Outdoors_postprocess.csv"
elif [[ $workload == "m1" ]]; then
  post_data_path="./dataset/merged/merged_sports_clothes_office_dblp.csv"
  aux_data_path="./dataset/merged/merged_sports_clothes_office_dblp.aux"
elif [[ $workload == "m2" ]]; then
  post_data_path="./dataset/merged/merged_dblp_sports_steam_movie.csv"
  aux_data_path="./dataset/merged/merged_dblp_sports_steam_movie.aux"
# elif [[ $workload == "m3" ]]; then
#   post_data_path="./dataset/merged/merged_alibaba_anime_movie_steam.csv"
#   aux_data_path="./dataset/merged/merged_alibaba_anime_movie_steam.aux"
elif [[ $workload == "m4" ]]; then
  post_data_path="./dataset/merged/merged_twitch_movie_anime_steam.csv"
  aux_data_path="./dataset/merged/merged_twitch_movie_anime_steam.aux"
elif [[ $workload == "m5" ]]; then
  post_data_path="./dataset/merged/merged_clothes_office_dblp_anime.csv"
  aux_data_path="./dataset/merged/merged_clothes_office_dblp_anime.aux"
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
