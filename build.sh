if [ -d "/tmp/logroot" ];
then
	#rm /tmp/logroot -rf;
	echo "";
fi

if [ ! -d "build" ];
then
	mkdir build
	echo 'DIR ./build created.'
else
	echo 'DIR ./build exists.'
fi


cd build
rm * -rf

cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build ./

# ./cylog

cd ../

echo ""

