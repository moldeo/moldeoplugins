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

(mv *.tar.gz ./deb/) > /dev/null || {
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
( rename 's/erase/moldeo\-erase/' deb/erase-* ) > /dev/null || {
  echo
  echo "**Error**: renaming."
  echo ""
  exit 1
}

echo " 
Now execute in deb folder:
 cd deb/*-*
 dh_make -l -e fabricio.costa@moldeointeractive.com.ar -p moldeo-erase -f ../erase_
 cp ../../control.amd64 debian/control
 cp ../../moldeo-erase1.dirs.amd64 debian/moldeo-erase1.dirs
 cp ../../moldeo-erase1.install.amd64 debian/moldeo-erase1.install
 cp ../../moldeo-erase-dev.dirs.amd64 debian/moldeo-erase-dev.dirs
 cp ../../moldeo-erase-dev.install.amd64 debian/moldeo-erase-dev.install
 vi debian/changelog
 dpkg-buildpackage -us -uc -rfakeroot 2>&1 | tee ../../buildpkg_logs.txt
"


echo "Success: extraction"
exit 0
