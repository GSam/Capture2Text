git clone https://github.com/wine-mirror/wine.git
sudo apt-get install flex bison -y
cd wine
./configure --enable-win64 --without-x --without-freetype
make -j2
