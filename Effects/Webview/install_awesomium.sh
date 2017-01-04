AWVERSION=1_7_5
AWVERSION=1_7_5
AWOS=linux64
AWPACKAGENAME=awesomium_$AWVERSION\_sdk_$AWOS
AWDOWNLOADSITE=http://www.awesomium.com/downloads
AWPACKAGEDOWNLOADNAME=$AWPACKAGENAME.tar.gz
AWPACKAGEDOWNLOADLINK=$AWDOWNLOADSITE/$AWPACKAGEDOWNLOADNAME
rm -Rf awesomium_*
wget $AWPACKAGEDOWNLOADLINK
tar -xvf $AWPACKAGEDOWNLOADNAME
cd awesomium_*_sdk_$AWOS
sudo apt-get install libsdl1.2-dev
sudo apt-get install libgcrypt*
sudo make install

