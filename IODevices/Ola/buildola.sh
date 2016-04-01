git clone https://github.com/OpenLightingProject/ola
cd ola
sudo apt-get install bison flex uuid-dev libcppunit-dev libprotoc-dev libftdi-dev liblo-dev libmicrohttpd-dev dh-autoreconf
automake --add-missing
autoreconf
./configure --prefix=/usr
make
sudo make install
