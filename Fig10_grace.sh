set -e
set -x

mkdir -p grace_cluster/
cd gapbs;
make
./run.sh steam 40 5
./run.sh anime 40 5
./run.sh movie 0 5
./run.sh dblp 50 3
./run.sh office 30 4
./run.sh twitch 40 5
./run.sh sports 10 4
./run.sh clothes 30 4
./run.sh M1 30 6
./run.sh M2 20 4
./run.sh M3 50 4
./run.sh M4 50 4
cp -r cluster/*.cluster ../grace_cluster/
cd -


./run_grace_hbm_only.sh steam 40 5
./run_grace_hbm_only.sh anime 40 5
./run_grace_hbm_only.sh movie 0 5
./run_grace_hbm_only.sh dblp 50 3
./run_grace_hbm_only.sh office 30 4
./run_grace_hbm_only.sh twitch 40 5
./run_grace_hbm_only.sh sports 10 4
./run_grace_hbm_only.sh clothes 30 4
./run_grace_hbm_only.sh M1 30 6
./run_grace_hbm_only.sh M2 20 4
./run_grace_hbm_only.sh M3 50 4
./run_grace_hbm_only.sh M4 50 4




echo "Fig. 10 GRACE finished"




