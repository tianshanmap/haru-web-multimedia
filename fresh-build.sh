rm -rf build/
rm -rf install/
mkdir install
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --prefix ./install
#cmake -S . -B build
#cmake --build build
#rm -rf bin/
#mkdir bin/
#cp build/web-multimedia bin/
cp config/* install/bin/
