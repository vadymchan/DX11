git submodule update --init --recursive
cd .\assimp\
cmake -DASSIMP_BUILD_ZLIB=ON CMakeLists.txt