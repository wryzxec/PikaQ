set -e

rm -rf build

mkdir build
cd build
cmake ..
make

ctest --verbose
