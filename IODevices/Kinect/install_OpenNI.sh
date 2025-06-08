
# http://www.openni.org/openni-sdk/openni-sdk-history-2/

sudo apt-get install libusb-1.0-0-dev freeglut3-dev g++

cp Platform.Arm.RPI3 ~/Platform.Arm
cd ~
mkdir kinect
cd kinect
git clone https://github.com/OpenNI/OpenNI.git -b unstable
cd ~/kinect/OpenNI/Platform/Linux/CreateRedist
cp ~/Platform.Arm ~/kinect/OpenNI/Platform/Linux/Build/Common/Platform.Arm
bash RedistMaker
cd ../Redist/OpenNI-Bin-Dev-Linux-*/
sudo ./install.sh

#wget http://www.openni.org/wp-content/uploads/2012/12/OpenNI-Bin-Dev-Linux-x64-v1.5.4.0.tar.zip
#unzip OpenNI-Bin-Dev-Linux-x64-v1.5.4.0.tar.zip
#tar xvjf OpenNI-Bin-Dev-Linux-x64-v1.5.4.0.tar.bz2
#mv OpenNI-Bin-Dev-Linux-x64-v1.5.4.0 openni


#wget https://github.com/avin2/SensorKinect/archive/unstable.zip
#unzip SensorKinect-unstable.zip
#mv SensorKinect-unstable sensorkin
#git clone https://github.com/avin2/SensorKinect -b unstable
cd ~/kinect
git clone https://github.com/ph4m/SensorKinect.git -b unstable
cd ~/kinect/SensorKinect/Platform/Linux/CreateRedist
bash RedistMaker
cd ../Redist/Sensor-Bin-Linux-*
sudo sh install.sh

#wget http://www.openni.org/wp-content/uploads/2012/12/Sensor-Bin-Linux-x64-v5.1.2.1.tar.zip
#unzip Sensor-Bin-Linux-x64-v5.1.2.1.tar.zip
#tar xvjf Sensor-Bin-Linux-x64-v5.1.2.1.tar.bz2

#wget http://www.openni.org/wp-content/uploads/2012/12/NITE-Bin-Linux-x64-v1.5.2.21.tar.zip
#unzip NITE-Bin-Linux-x64-v1.5.2.21.tar.zip
#tar xvjf NITE-Bin-Linux-x64-v1.5.2.21.tar.bz2
#mv NITE-Bin-Linux-x64-v1.5.2.21 nite
cd ~/kinect
wget https://www.moldeo.org/archivos/binario/nite-bin-linux-x64-v1.5.2.21.tar.bz2
tar -xvf nite-bin-linux-x64-v1.5.2.21.tar.bz2
mv NITE-Bin-Dev-Linux-x64-v1.5.2.21 nite
cd ~/kinect/nite/
chmod a+x install.sh
sudo ./install.sh


#cd ~/kinect/openni/
#chmod a+x install.sh
#sudo ./install.sh

#cd ~/kinect/sensorkin/
#chmod a+x install.sh
#sudo ./install.sh





