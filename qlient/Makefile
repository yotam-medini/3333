# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2024/October/21

SHELL=/bin/bash
QT=${HOME}/Qt/6.8.0

clean: clean-desktop clean-wasm

# Desktop
clean-desktop:
	rm -rf build
desktop: desktop-cmake desktop-make
desktop-cmake:
	mkdir -p build
	(cd build; ${QT}/gcc_64/bin/qt-cmake -DCMAKE_BUILD_TYPE=Debug ..)
desktop-make:
	(cd build; make)

# Wasm
EMSDK=${HOME}/pub/emsdk
EMSDK_ENV=${EMSDK}/emsdk_env.sh
QT_WASM=${QT}/wasm_singlethread/
clean-wasm:
	rm -rf build-wasm
wasm: wasm-cmake wasm-make
wasm-cmake:
	(. ${EMSDK_ENV}; mkdir -p build-wasm; cd build-wasm; \
	  ${QT_WASM}/bin/qt-cmake -DCMAKE_BUILD_TYPE=Debug ..)
wasm-make:
	(source ${EMSDK_ENV}; cd build-wasm; emmake make -s LOGLEVEL=4)

# Golden-test
golden-test: ./bin.d/golden-test
	./bin.d/golden-test 60 80 3
	./bin.d/golden-test 80 60 3
	./bin.d/golden-test 60 80 12
	./bin.d/golden-test 80 60 12

./bin.d/golden-test: golden.cpp golden.h
	@mkdir -p ${@D}
	g++ -DGOLDEN_TEST=1 -Wall -g -o $@ golden.cpp -lfmt
