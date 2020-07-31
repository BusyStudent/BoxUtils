#Script to release code on linux
xmake project -k vs2019
xmake project -k cmake
xmake project -k makefile
cd ./src && make clean && cd ../

tar -cjf libbox_code.tar.bz2 ./image/ ./include ./src/ \
./tests/ ./spider ./SDL ./CMakeLists.txt ./LICENSE ./README.md ./xmake.lua \
./vs2019 ./makefile ./uv