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

if [[ $workload == "anime" ]]; then
  data_path="./dataset/anime/rating_preprocess.csv"
  post_data_path="./dataset/anime/rating_postprocess.csv"
elif [[ $workload == "steam" ]]; then
  data_path="./dataset/steam/australian_users_items_preprocess.csv"
  post_data_path="./dataset/steam/australian_users_items_postprocess.csv"
elif [[ $workload == "movie" ]]; then
  data_path="./dataset/movie/rating_preprocess.csv"
  post_data_path="./dataset/movie/rating_postprocess.csv"
elif [[ $workload == "twitch" ]]; then
  data_path="./dataset/twitch/100k_a_preprocess.csv"
  post_data_path="./dataset/twitch/100k_a_postprocess.csv"
else
    echo "invalid workload given in argv 1"
    exit 0
fi


g++ -g -std=c++11 -O3 -funroll-loops -fopenmp sls_clean_dataset.cc -o bin/sls_clean_${workload}
./bin/sls_clean_${workload} ${data_path} > ${post_data_path}


wait
echo "${workload} done!"
