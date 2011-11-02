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


echo " 
Now execute in deb folder:
 cd deb/moldeoplugins-*
 dh_make -l -e fabricio.costa@moldeointeractive.com.ar -p moldeoplugins
 cp ../../control.amd64 debian/control
 cp ../../rules.amd64 debian/rules
 cp ../../moldeoplugins1.dirs.amd64 debian/moldeoplugins.dirs 
 cp ../../moldeoplugins1.install.amd64 debian/moldeoplugins.install
 cp ../../moldeoplugins-dev.dirs.amd64 debian/moldeoplugins-dev.dirs
 cp ../../moldeoplugins-dev.install.amd64 debian/moldeoplugins-dev.install
 vi debian/changelog
 dpkg-buildpackage -us -uc -rfakeroot 2>&1 | tee ../../buildpkg_logs.txt
"


echo "Success: extraction"
exit 0
