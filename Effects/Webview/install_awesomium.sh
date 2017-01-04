AWVERSION=1_7_5
AWVERSION=1_7_5
AWOS=linux64
AWPACKAGENAME=awesomium_$AWVERSION\_sdk_$AWOS
AWDOWNLOADSITE=http://www.awesomium.com/downloads
AWPACKAGEDOWNLOADNAME=$AWPACKAGENAME.tar.gz
AWPACKAGEDOWNLOADLINK=$AWDOWNLOADSITE/$AWPACKAGEDOWNLOADNAME
GCRYPT11NAME=libgcrypt11_1.5.3-2ubuntu4.2_amd64.deb
#GCRYPT11NAME32=libgcrypt11_1.5.3-2ubuntu4.2_i386.deb
#GCRYPT11LINK=https://launchpad.net/ubuntu/+archive/primary/+files/$GCRYPT11NAME
GCRYPT11LINK=http://www.moldeo.org/archivos/binario/$GCRYPT11NAME
rm -Rf awesomium_*
wget $GCRYPT11LINK
sudo dpkg -i $GCRYPT11NAME
wget $AWPACKAGEDOWNLOADLINK
tar -xvf $AWPACKAGEDOWNLOADNAME
cd awesomium_*_sdk_$AWOS
sudo apt-get install libsdl1.2-dev
sudo apt-get install libgcrypt*
sudo make all
sudo make install

