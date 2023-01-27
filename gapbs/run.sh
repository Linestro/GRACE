set -Eeuo pipefail
set -e
set -x

mkdir -p cluster
mkdir -p bin

workload=$1
decay=$2
length=$3
ps=(50)

cp ./analyze ./bin/analyze_${workload}
./bin/analyze_${workload} -sf ../graph/${workload}_50_percent_sampling_graph.wel ${workload} ${decay} ${length}


wait


echo "gapbs $1 complete"

