# set -e
# set -x


cd metis/metis-5.1.0;
pip install metis # Verified on python 3.8.10
make clean
make config shared=1
sudo make install
cd -;



