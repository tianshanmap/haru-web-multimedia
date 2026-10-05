rm -rf build/
cmake -S . -B build
cmake --build build
rm -rf bin/
mkdir bin/
cp build/web-multimedia bin/
cp config/* bin/

