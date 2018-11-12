#!/bin/bash
script_name=$(readlink -e $0)
script_dir=$(dirname $script_name)
prj_dir=$(dirname $script_dir)

rm --force ./cppcheck.cppcheck.log
$script_dir/cppcheck.sh "$prj_dir" "$prj_dir/.ci/suppressions.txt" $@ |& tee ./cppcheck.cppcheck.log
sh -c "! grep '\[' ./cppcheck.cppcheck.log" > /dev/null
res=$?
rm --force ./cppcheck.cppcheck.log
exit $res
