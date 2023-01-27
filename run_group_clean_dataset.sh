set -Eeuo pipefail
set -x

./run_clean_dataset.sh anime &
./run_clean_dataset.sh steam &
./run_clean_dataset.sh movie &
./run_clean_dataset.sh twitch &
wait

echo "Done clean dataset"