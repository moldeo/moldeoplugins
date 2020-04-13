# Awesomium replaced by Ultralight
# https://ultralig.ht/ultralight-sdk-0.9.1-linux-x64.tar.xz
ULVERSION=0.9.1
ULOS=linux-x64
ULPACKAGENAME=ultralight-sdk-$ULVERSION-$ULOS
ULDOWNLOADSITE=https://ultralig.ht/
ULPACKAGEDOWNLOADNAME=$ULPACKAGENAME.tar.xz
ULPACKAGEDOWNLOADLINK=$ULDOWNLOADSITE/$ULPACKAGEDOWNLOADNAME
#GCRYPT11NAME=libgcrypt11_1.5.3-2ubuntu4.2_amd64.deb
#GCRYPT11NAME32=libgcrypt11_1.5.3-2ubuntu4.2_i386.deb
#GCRYPT11LINK=https://launchpad.net/ubuntu/+archive/primary/+files/$GCRYPT11NAME
#GCRYPT11LINK=http://www.moldeo.org/archivos/binario/$GCRYPT11NAME
rm -Rf ultralight*
#wget $GCRYPT11LINK
#sudo dpkg -i $GCRYPT11NAME
wget $ULPACKAGEDOWNLOADLINK
tar -xvf $ULPACKAGEDOWNLOADNAME
cd ultralight*$ULOS
#sudo apt-get install libsdl1.2-dev
#sudo apt-get install libgcrypt*
#sudo make all
#sudo make install
mkdir build
cd build
cmake ..
cmake --build . --config Release
