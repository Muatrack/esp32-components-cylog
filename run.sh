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
# ./cylog

cd ../



