set -e
set -x
if [[ $# -eq 0 ]] ; then
    echo 'give me workload'
    exit 0
fi


workload=$1
# hbm_cap=$2


mkdir -p hbm_only_metis_log/

g++ -fopenmp -O3 hbm_only_metis_inference_cnt.cc -o ./bin/metis_hbm_only_cnt_${workload}.out

./bin/metis_hbm_only_cnt_${workload}.out ${workload} post_trace/${workload}.trace



wait

echo "Finished"