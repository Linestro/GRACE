# set -x

count_lines(){
    if [[ $workload == "anime" ]]; then
    post_data_path="./dataset/anime/rating_postprocess.csv"
    elif [[ $workload == "office" ]]; then
    post_data_path="./dataset/office/amazon_Office_Products_postprocess.csv"
    elif [[ $workload == "dblp" ]]; then
    post_data_path="./dataset/dblp/dblp_postprocess.csv"
    elif [[ $workload == "sports" ]]; then
    post_data_path="./dataset/sports/amazon_Sports_and_Outdoors_postprocess.csv"
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
    wc -l ${post_data_path}
}


workloads=("anime" "steam" "movie" "clothes" "twitch" "sports" "office" "dblp")

for workload in "${workloads[@]}"
do
    echo "Count: ${workload}"
    count_lines ${workload}
done