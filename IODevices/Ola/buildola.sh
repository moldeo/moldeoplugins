git clone https://github.com/OpenLightingProject/ola
cd ola
sudo apt-get install bison flex uuid-dev libcppunit-dev libprotoc-dev libftdi-dev liblo-dev libmicrohttpd-dev dh-autoreconf
automake --add-missing
autoreconf
./configure --prefix=/usr
make
sudo make install

echo "Use ola_dev_info to start and update the olad(daemon) service"
ola_dev_info
echo "Set the number of universes (or input/output ports) you will use for each protocol, check '~/.ola/ola*.conf' as '~/.ola/ola-e131.conf': "
sudo nano ~/.ola/ola-e131.conf
