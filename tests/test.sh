#!/usr/bin/bash
build_dir=$1

result_file="result.txt"
touch $build_dir/test/$result_file

$build_dir/test/test_suite_uniq.sh test_very_simple $result_file $build_dir> /dev/null
$build_dir/test/test_suite_dup.sh test_very_simple $result_file $build_dir> /dev/null
#./test_suite.sh $perfomance_enabled test_grpc_no_long_str result.txt > /dev/null
#./test_suite.sh $perfomance_enabled test_grpc result.txt > /dev/null

cat $build_dir/test/$result_file

rm $build_dir/test/$result_file