#!/usr/bin/bash

build_dir=$3

echo "Start $1 filename $2"

test_name=$1 #also test name
common_result_file=$2

$build_dir/dupl -f $build_dir/test/$test_name/test.txt -u | grep -v "Unique lines:"> $build_dir/test/$test_name/test_res_uniq.txt
diff $build_dir/test/$test_name/test_uniq.txt $build_dir/test/$test_name/test_res_uniq.txt > $build_dir/test/$test_name/test_diff_result.txt

result=$?
if [ $result -eq 0 ];
then
	echo SUCCESS
	echo -e "$test_name unique \t\t\t- SUCCESS" >> $build_dir/test/$common_result_file
else
	echo FAIL
	echo -e "$test_name unique \t\t\t- FAIL" >> $build_dir/test/$common_result_file
fi

rm $build_dir/test/$test_name/test_diff_result.txt