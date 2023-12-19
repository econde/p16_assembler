#!/bin/bash

if [ $# -ne 3  ]; then
	echo "usage: $0 <files widthout erros> <files width only warnings> <files width erros>"
	exit 1
fi

PAS="build/p16as"

rm -rf test/build
mkdir -p test/build

sources_no_errors=($(cat "$1"))
sources_warnings=($(cat "$2"))
sources_errors=($(cat "$3"))

echo "-------------------Programas sem erros-----------------------------------"

for file in "${sources_no_errors[@]}"; do
	echo $file
	source="${file%.*}"
	$PAS test/$source.s -o test/build/$source
	if [ $? -ne 0 ]; then
		echo "Error in "$source.s
	fi
	cmp test/build/$source.hex test/reference/$source.hex
done

echo "----------------------Programas apenas com warnings----------------------"

for file in "${sources_warnings[@]}"; do
	echo $file
	source="${file%.*}"
	$PAS test/$source.s -o test/build/$source 2> test/build/$source.out
	if [ $? -ne 0 ]; then
		echo "Error in "$source.s
	fi
	cmp test/build/$source.hex test/reference/$source.hex
	cmp <(tail -n +2 test/build/$source.lst) <(tail -n +2 test/reference/$source.lst)
	cmp test/build/$source.out test/reference/$source.out
done

echo "-------------------Programas com erros-----------------------------------"

for file in "${sources_errors[@]}"; do
	echo $file
	source="${file%.*}"
	$PAS test/$source.s -o test/build/$source 2> test/build/$source.out
	if [ $? -ne 0 ]; then
		echo "Error in "$source.s
	fi
	cmp test/build/$source.out test/reference/$source.out
done

$PAS samples/div/div16.s -o test/build/div16
cmp test/build/div16.hex test/reference/div16.hex
if [ $? -ne 0 ]; then
	echo "Error in test/build/div16.hex"
fi
cmp <(tail -n +2 test/build/div16.lst) <(tail -n +2  test/reference/div16.lst)
if [ $? -ne 0 ]; then
	echo "Error in test/build/div16.lst"
fi

$PAS samples/find_generic/find_generic.s -o test/build/find_generic
cmp test/build/find_generic.hex test/reference/find_generic.hex
if [ $? -ne 0 ]; then
	echo "Error in test/build/find_generic.hex"
fi
cmp <(tail -n +2 test/build/find_generic.lst) <(tail -n +2 test/reference/find_generic.lst)
if [ $? -ne 0 ]; then
	echo "Error in test/build/find_generic.lst"
fi

$PAS samples/search/search.s -o test/build/search
cmp test/build/search.hex test/reference/search.hex
if [ $? -ne 0 ]; then
	echo "Error in test/build/search.hex"
fi
cmp <(tail -n +2 test/build/search.lst) <(tail -n +2  test/reference/search.lst)
if [ $? -ne 0 ]; then
	echo "Error in test/build/search.lst"
fi
