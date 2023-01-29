
git clone https://github.com/Linestro/GRACE.git
cd GRACE

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

### Preparing Single Datasets

## DBLP, Sports, Office, Clothes [30 minutes]
git clone https://github.com/SNU-ARC/MERCI.git

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
python port_dataset.py dblp
python port_dataset.py amazon_Office_Products
python port_dataset.py amazon_Clothing_Shoes_and_Jewelry
python port_dataset.py amazon_Sports_and_Outdoors

mkdir -p $HOME/dataset/dblp
mkdir -p $HOME/dataset/office
mkdir -p $HOME/dataset/clothes
mkdir -p $HOME/dataset/sports

mv dblp_postprocess.csv dataset/dblp/
mv amazon_Office_Products_postprocess.csv dataset/office/
mv amazon_Clothing_Shoes_and_Jewelry_postprocess.csv dataset/clothes/
mv amazon_Sports_and_Outdoors_postprocess.csv dataset/sports/

## Anime [5 minutes]
cd $HOME/dataset/anime
Download from https://www.kaggle.com/datasets/CooperUnion/anime-recommendations-database?resource=download
cd -

## Twitch [5 minutes]
cd $HOME/dataset/twitch
Download from https://drive.google.com/drive/folders/1BD8m7a8m7onaifZay05yYjaLxyVV40si
cd -

## Movie [5 minutes]
cd $HOME/dataset/movie
wget https://files.grouplens.org/datasets/movielens/ml-20m.zip
cd -

## Steam [5 minutes]
cd $HOME/dataset/steam
Download from https://drive.google.com/file/d/1pZxvHa3yLzIAE15TYDQI7G5qrI8rJab_/view
cd -



cd dataset

## Anime [10 minutes]
cd anime && unzip archive.zip && sed -i '1d' rating.csv && cd -
python dataset_preprocess.py -i anime/rating.csv

## Twitch [5 minutes]
python dataset_preprocess.py -i twitch/100k_a.csv

## Movie [5 minutes]
cd movie && unzip ml-20m.zip && mv ml-20m/* . && rm -r ml-20m && sed -i '1d' ratings.csv && mv ratings.csv rating.csv && cd -
python dataset_preprocess.py -i movie/rating.csv

## Steam [120 minutes]
cd steam && gunzip australian_users_items.json.gz && cd -
python steam_preprocess.py -i steam/australian_users_items.json



cd $HOME

## Apply MERCI Dataset Cleaning [10 minutes]
./run_group_clean_dataset.sh

## Verify datasets (optional)
./count_lines_in_dataset.sh # For dataset verification, check the output of the script against ./files.count

### Preparing Mixed Datasets [120 minutes]
./run_merge_all.sh

### Generate ICG with training set [180 minutes]
./run_build_graph_all.sh

### Reformat into inference streaming set [5 minutes]
./run_group_reformat.sh

### Inference with HBM only count (GRACE) [10 minutes]
./Fig10_grace.sh

### Inference with HBM only count (MERCI) [30 minutes]
./Fig10_merci.sh

### Inference with HBM only count (SPACE) [10 minutes]
./Fig10_space.sh

### Inference with HBM only count (Metis) (optional) [360 minutes]
./Fig10_metis.sh

### Plot figure [1 minute]
cd Fig10_plot
python fig10.py
python fig10_without_Metis.py [if skip Metis]

### Result figure will be in fig10.png






