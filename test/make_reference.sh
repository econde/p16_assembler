#!/bin/bash

if [ $# -ne 3  ]; then
	echo "usage: $0 <files widthout erros> <files width only warnings> <files width erros>"
	exit 1
fi

PAS="build/p16as"

rm -rf test/reference
mkdir -p test/reference

sources_no_errors=($(cat "$1"))
sources_warnings=($(cat "$2"))
sources_errors=($(cat "$3"))

for file in "${sources_no_errors[@]}"; do
	echo $file
	source="${file%.*}"
	$PAS test/$source.s -o test/reference/$source
	if [ $? -ne 0 ]; then
		echo "Error in "$source.s
	fi
done

for file in "${sources_warnings[@]}"; do
	echo $file
	source="${file%.*}"
	$PAS test/$source.s -o test/reference/$source 2> test/reference/$source.out
	if [ $? -ne 0 ]; then
		echo "Error in "$source.s
	fi
done

for file in "${sources_errors[@]}"; do
	echo $file
	source="${file%.*}"
	$PAS test/$source.s -o test/reference/$source 2> test/reference/$source.out
	if [ $? -ne 0 ]; then
		echo "Error in "$source.s
	fi
done

$PAS samples/div/div16.s -o test/reference/div16
$PAS samples/find_generic/find_generic.s -o test/reference/find_generic
$PAS samples/search/search.s -o test/reference/search
