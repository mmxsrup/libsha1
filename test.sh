#!/bin/bash

gen_data_files() {
	dd if=/dev/urandom of=./tmp bs=4K count=1
}

check() {
	diff <(sha1sum memo.txt) <(LD_LIBRARY_PATH=. ./test memo.txt)
}


try() {

	for (( t=0; t<100; t++ ))
	do
		gen_data_files
		check
		if [ $? = 0 ]; then
			echo $t": Pass"
		else
			echo "Wrong"
			exit 1
		fi
	done

}

try
echo OK
