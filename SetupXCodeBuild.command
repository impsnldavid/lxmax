 #!/bin/bash

SOURCEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "${SOURCEDIR}"

if [ ! -d build ]; then
	mkdir build
fi

cd build

echo
echo
echo "-----------------------------------------"
echo "Installing conan dependencies..."
echo "-----------------------------------------"
sudo conan install .. -g cmake_multi -s build_type=Debug -s compiler="apple-clang" --build=missing
sudo conan install .. -g cmake_multi -s build_type=Release -s compiler="apple-clang" --build=missing

echo
echo
echo "-----------------------------------------"
echo "Running CMake..."
echo "-----------------------------------------"
cmake -G Xcode ..

echo
echo
echo "-----------------------------------------"
echo "Build ready!"
echo "-----------------------------------------"
