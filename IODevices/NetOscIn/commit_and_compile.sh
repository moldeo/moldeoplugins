git commit -am "Commit and compile"
git push
make -j8

# Check the exit status
if [ $? -eq 0 ]; then
        echo "Make succeeded!"

	sudo make install

	echo "--------------------"
	echo "####################"
	echo "Ready to call Moldeo"
	echo "####################"
	echo "--------------------"

	moldeoplayersdl2 -mol ~/moldeo/moldeoinstaller/moldeosamples/basic/00_Image/00_Image.mol
else
	echo "!!!!!!!!!!!!!!!!!!!!"
	echo "XXXXXXXXXXXXXXXXXXXX"
	echo "Make failed!"
	echo "XXXXXXXXXXXXXXXXXXXX"
	echo "¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡¡"

fi

