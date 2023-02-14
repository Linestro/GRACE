## Required packages:

gcc, cmake, python3 (Python 3.9 preferred)

## **Step 1**: Create necessary folders GRACE/ and GRACE/MERCI/

git clone https://github.com/Linestro/GRACE.git

cd GRACE

pip3 install -r requirement.txt

git clone https://github.com/SNU-ARC/MERCI.git

mkdir -p bin/

mkdir -p gapbs/bin/

mkdir -p gapbs/cluster/

mkdir -p dataset/anime

mkdir -p dataset/clothes

mkdir -p dataset/dblp

mkdir -p dataset/movie

mkdir -p dataset/office

mkdir -p dataset/sports

mkdir -p dataset/steam

mkdir -p dataset/twitch

export HOME=$(pwd)

## **Step 2**: Download and process datasets

### DBLP, Sports, Office, Clothes [30 minutes]

cd MERCI

mkdir -p data/1_raw_data/dblp

mkdir -p data/1_raw_data/amazon

mkdir -p data/2_transactions/

mkdir -p data/3_train_test/

mkdir -p data/4_filtered/amazon_Office_Products

mkdir -p data/4_filtered/amazon_Clothing_Shoes_and_Jewelry

mkdir -p data/4_filtered/amazon_Sports_and_Outdoors

cd data/1_raw_data/dblp;

wget https://nrvis.com/download/data/ca/ca-coauthors-dblp.zip

unzip ca-coauthors-dblp.zip

cd -

cd data/1_raw_data/amazon;

wget --no-check-certificate https://jmcauley.ucsd.edu/data/amazon_v2/categoryFiles/Sports_and_Outdoors.json.gz

wget --no-check-certificate https://jmcauley.ucsd.edu/data/amazon_v2/metaFiles2/meta_Sports_and_Outdoors.json.gz

wget --no-check-certificate https://jmcauley.ucsd.edu/data/amazon_v2/categoryFiles/Office_Products.json.gz

wget --no-check-certificate https://jmcauley.ucsd.edu/data/amazon_v2/metaFiles2/meta_Office_Products.json.gz

wget --no-check-certificate https://jmcauley.ucsd.edu/data/amazon_v2/categoryFiles/Clothing_Shoes_and_Jewelry.json.gz

wget --no-check-certificate https://jmcauley.ucsd.edu/data/amazon_v2/metaFiles2/meta_Clothing_Shoes_and_Jewelry.json.gz

cd $HOME/MERCI/1_preprocess/scripts/

./lastfm_dblp.sh dblp

python3 amazon_parse_divide_filter.py Office_Products

python3 amazon_parse_divide_filter.py Clothing_Shoes_and_Jewelry

python3 amazon_parse_divide_filter.py Sports_and_Outdoors

cd $HOME/

python3 port_dataset.py dblp

python3 port_dataset.py amazon_Office_Products

python3 port_dataset.py amazon_Clothing_Shoes_and_Jewelry

python3 port_dataset.py amazon_Sports_and_Outdoors

mkdir -p $HOME/dataset/dblp

mkdir -p $HOME/dataset/office

mkdir -p $HOME/dataset/clothes

mkdir -p $HOME/dataset/sports

mv dblp_postprocess.csv dataset/dblp/

mv amazon_Office_Products_postprocess.csv dataset/office/

mv amazon_Clothing_Shoes_and_Jewelry_postprocess.csv dataset/clothes/

mv amazon_Sports_and_Outdoors_postprocess.csv dataset/sports/

### Anime [5 minutes]
cd $HOME/dataset/anime

Download from https://www.kaggle.com/datasets/CooperUnion/anime-recommendations-database?resource=download

cd -

### Twitch [5 minutes]
cd $HOME/dataset/twitch

Download from https://drive.google.com/drive/folders/1BD8m7a8m7onaifZay05yYjaLxyVV40si

cd -

### Movie [5 minutes]
cd $HOME/dataset/movie

wget https://files.grouplens.org/datasets/movielens/ml-20m.zip

cd -

### Steam [5 minutes]
cd $HOME/dataset/steam

Download from https://drive.google.com/file/d/1pZxvHa3yLzIAE15TYDQI7G5qrI8rJab_/view

cd -


## Preprocess datasets
cd dataset

### Anime [10 minutes]
cd anime && unzip archive.zip && sed -i '1d' rating.csv && cd -

python3 dataset_preprocess.py -i anime/rating.csv

### Twitch [5 minutes]
python3 dataset_preprocess.py -i twitch/100k_a.csv

### Movie [5 minutes]
cd movie && unzip ml-20m.zip && mv ml-20m/* . && rm -r ml-20m && sed -i '1d' ratings.csv && mv ratings.csv rating.csv && cd -

python3 dataset_preprocess.py -i movie/rating.csv

### Steam [120 minutes]
cd steam && gunzip australian_users_items.json.gz && cd -

python3 steam_preprocess.py -i steam/australian_users_items.json



cd $HOME

## **Step 3**: Perform datasets cleaning same as MERCI [10 minutes]
./run_group_clean_dataset.sh

## Verify datasets (optional)
./count_lines_in_dataset.sh     # For dataset verification, check the output of the script against ./files.count

## **Step 4**: Prepare mixed datasets [120 minutes]
./run_merge_all.sh

## **Step 5**: Generate ICG with training set [180 minutes]
./run_build_graph_all.sh

## **Step 6**: Reformat datasets into inference streaming set [5 minutes]
./run_group_reformat.sh

## **Step 7**: Reproduce memory access count for GRACE, MERCI, SPACE

## Inference with HBM only count (GRACE) [10 minutes]
./Fig10_grace.sh

## Inference with HBM only count (MERCI) [30 minutes]
./Fig10_merci.sh

## Inference with HBM only count (SPACE) [10 minutes]
./Fig10_space.sh

## **Step 8**: Reproduce memory access count for Metis (optional)

## Install Metis library
sudo ./metis_installation.sh    # make install metis needs sudo

## Inference with HBM only count (Metis) (optional) [360 minutes]
./Fig10_metis.sh

## **Step 9**: Reproduce Fig. 10 in the paper using the memory access count collected in steps 7 and 8 [1 minute]
cd Fig10_plot

python3 fig10.py

python3 fig10_without_Metis.py [if skip Metis]

## Result figure will be in fig10.png






