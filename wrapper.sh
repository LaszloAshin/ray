#!/bin/sh
tar xzf src.tar.gz
make &> compile.log
if [ $? -ne 0 ]; then
	echo "forditasi hiba"
	exit
fi
./ray ${1}
mv tracement.tga tracement${1}.tga
