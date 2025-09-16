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

cmake ../
cmake --build ./

./cylog

cd ../

echo "-------------- read alarm log file--------------"
cat /tmp/logroot/alarm/alm_00
echo ""

