#!/bin/sh

build_path=$1
build_arch=$2
build_type=$3

if test $# -lt 2; then
	echo "$0 <src path> <toolchain file> [Release|Debug]"
else
	build_path=${build_path:-"../src"}

	cmake -DCMAKE_TOOLCHAIN_FILE=${build_arch:-"$build_path/cmake/toolchain-gcc.cmake"} -DCMAKE_BUILD_TYPE=${build_type:-"Release"} $build_path
fi
