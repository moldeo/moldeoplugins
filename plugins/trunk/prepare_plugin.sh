#!/bin/bash

TARGETPATH=$1
PLUGINTITLE=$2
PLUGINNAME=$3
PLUGINTYPE=$4

SVN="svn"
M4="m4"

if [ -d $TARGETPATH/src ]; then
	for file in $TARGETPATH/src/*.cpp; do
		filename=$(basename $file)
		$SVN mv $file $TARGETPATH/$filename
	done
	$SVN rm $TARGETPATH/src
fi

if [ -d $TARGETPATH/inc ]; then
	for file in $TARGETPATH/inc/*.h; do
		filename=$(basename $file)
		$SVN mv $file $TARGETPATH/$filename
	done
	$SVN rm $TARGETPATH/inc
fi

TEMPLATEPATH=./TemplatePlugin
FILES="AUTHORS autogen.sh configure.ac COPYING INSTALL Makefile.am missing NEWS README ltmain.sh ChangeLog"
M4FILES="Makefile.am configure.ac"

for file in $FILES; do
	if [ ! -f $TARGETPATH/$file ]; then
		$SVN cp $TEMPLATEPATH/$file $TARGETPATH/.
	fi
done

pushd $TARGETPATH
PLUGINSOURCES=$(echo *.cpp *.c)
popd

for file in $M4FILES; do
	echo "M4 $file"
	$M4 -DPLUGINTITLE=$PLUGINTITLE \
		-DPLUGINNAME=$PLUGINNAME \
		-DPLUGINSOURCES="$PLUGINSOURCES" \
		-DPLUGINTYPE=$PLUGINTYPE \
		$TEMPLATEPATH/$file > $TARGETPATH/$file
done

pushd $TARGETPATH
$SVN rm *.cbp
$SVN rm *.dsp
popd


