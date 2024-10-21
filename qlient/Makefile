# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2024/October/21

QT=${HOME}/Qt/6.8.0

clean:
	rm -rf build

desktop:
	mkdir -p build
	(cd build; ${QT}/gcc_64/bin/qt-cmake -DCMAKE_BUILD_TYPE=Debug ..)
	(cd build; make)
