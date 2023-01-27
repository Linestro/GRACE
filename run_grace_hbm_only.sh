set -e
set -x

if [[ $# -eq 0 ]] ; then
    echo 'give me workload'
    exit 0
fi


workload=$1
decay=$2
length=$3


mkdir -p hbm_only_grace_log/


g++ -fopenmp -O3 hbm_only_grace_inference_cnt.cc -o ./bin/grace_hbm_only_cnt_${workload}.out
./bin/grace_hbm_only_cnt_${workload}.out ${workload} post_trace/${workload}.trace ${decay} ${length}



wait

echo "${workload} Finished"