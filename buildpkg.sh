#!/bin/sh

DIE=0

echo "deb directory..."
(mkdir deb ) > /dev/null || {
	echo "cleaning..."

	(rm -Rf deb/*) > /dev/null || {
	  echo
	  echo "**Error**: removing error ."
	  echo ""
	  exit 1
	}
}



echo "making distribution source file..."
(make dist) > /dev/null || {
  echo
  echo "**Error**: make dist ."
  echo ""
  exit 1
}


echo "copying..."

(mv moldeoplugins-*.tar.gz ./deb/) > /dev/null || {
  echo
  echo "**Error**: moving file ."
  echo ""
  exit 1
}


echo "renaming..."
( rename 's/\.tar.gz/\.orig.tar.gz/' deb/*.tar.gz ) > /dev/null || {
  echo
  echo "**Error**: renaming ."
  echo ""
  exit 1
}
( rename 's/\-/\_/' deb/*.tar.gz ) > /dev/null || {
  echo
  echo "**Error**: renaming ."
  echo ""
  exit 1
}


echo "extracting..."

(tar -C deb -zxvf ./deb/*.orig.tar.gz ) > /dev/null || {
  echo
  echo "**Error**: extracting ."
  echo ""
  exit 1
}

cd deb/moldeoplugins-*
dh_make -l -e info@moldeointeractive.com.ar -p moldeoplugins
gedit ../../control.amd64.11.10 debian/control ../../moldeoplugins1.dirs.amd64 debian/moldeoplugins.dirs ../../moldeoplugins1.install.amd64 debian/moldeoplugins.install ../../moldeoplugins-dev.dirs.amd64 debian/moldeoplugins-dev.dirs ../../moldeoplugins-dev.install.amd64 debian/moldeoplugins-dev.install debian/changelog

echo " 
Now execute in deb folder:

in ubuntu 10.10:
 cp ../../control.amd64 debian/control

in ubuntu 9.10:
 cp ../../rules.amd64 debian/rules

in ubuntu 11.10:
 cp ../../control.amd64.11.10 debian/control

for all: 
 dpkg-buildpackage -us -uc -rfakeroot 2>&1 | tee ../../buildpkg_logs.txt
"


echo "Success: extraction"
exit 0
