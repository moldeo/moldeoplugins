#!/bin/bash

SVN="svn"
M4="m4"

convert_plugin() { 
	TARGETPATH=$1
	PLUGINNAME=$2
	PLUGINID=$3
	PLUGINTYPE=$4

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


	for file in $M4FILES; do
		$M4 -DPLUGINNAME=$PLUGINNAME \
			-DPLUGINID=$PLUGINID \
			-DPLUGINSOURCES="$PLUGINSOURCES" \
			-DPLUGINTYPE=$PLUGINTYPE \
			$TEMPLATEPATH/$file > $TARGETPATH/$file
	done

	pushd $TARGETPATH
	$SVN rm *.cbp
	$SVN rm *.dsp
	$SVN rm *.vcproj
	$SVN rm *.depend
	$SVN rm *.layout
	popd
}

commit() {
	pushd $pluginPath
	svn commit -m 'Plugin $pluginName usando autotools. sin compilar.'
	popd
}

updateM4() {
	TARGETPATH=$1
	PLUGINNAME=$2
	PLUGINID=$3
	PLUGINTYPE=$4
	M4FILES="Makefile.am"
	TEMPLATEPATH=./TemplatePlugin
	pushd $TARGETPATH
	PLUGINSOURCES=$(echo *.cpp)
	popd
	for file in $M4FILES; do
		$M4 -DPLUGINNAME=$PLUGINNAME \
			-DPLUGINID=$PLUGINID \
			-DPLUGINSOURCES="$PLUGINSOURCES" \
			-DPLUGINTYPE=$PLUGINTYPE \
			$TEMPLATEPATH/$file > $TARGETPATH/$file
	done
}

TYPESPATHS="MasterEffects Resources IODevices PreEffects Effects PostEffects"

for pluginTypePath in $TYPESPATHS; do
	PLUGINS=$pluginTypePath/*
	pluginType=$(echo $pluginTypePath | tr '[:upper:]' '[:lower:]')
	for pluginPath in $PLUGINS; do 
		pluginName=$(basename $pluginPath)
		pluginId=$(echo $pluginName | tr '[:upper:]' '[:lower:]')
		echo Processing: $pluginId 
		updateM4 $pluginPath $pluginName $pluginId $pluginType
		#commit($pluginPath)
		#convert_plugin $pluginPath $pluginName $pluginId $pluginType
	done
done
