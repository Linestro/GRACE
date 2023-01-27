set -e
set -x
# nvcc -Xcompiler -fopenmp -Xptxas -O3 merci_inference.cu -o ./bin/merci.out
if [[ $# -eq 0 ]] ; then
    echo 'give me workload'
    exit 0
fi

# make clean;
# make ./bin/merci.out

workload=$1
# hbm_cap=$2


mkdir -p hbm_only_merci_log/

g++ -fopenmp -O3 hbm_only_merci_inference_cnt.cc -o ./bin/merci_hbm_only_cnt_${workload}.out

./bin/merci_hbm_only_cnt_${workload}.out ${workload} post_trace/${workload}.trace



wait

echo "Finished"