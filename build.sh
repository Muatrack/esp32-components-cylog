if [ -d "/tmp/logroot" ];
then
	#rm /tmp/logroot -rf;
	echo "";
fi

if [ ! -d "out" ];
then
	mkdir out
	echo 'DIR ./build created.'
else
	echo 'DIR ./build exists.'
fi

rm out/* -rf

#cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake -S . -B out
cmake --build out

# ./cylog

#cd ../

echo ""

