#!/bin/bash
script_name=$(readlink -e $0)
script_dir=$(dirname $script_name)
prj_dir=$(dirname $script_dir)
prj=$(basename $prj_dir)

rm -r $prj_dir/build/report
lcov --no-external -d "$prj" -o $prj_dir/build/$prj.info -c -d $prj_dir/build/ 
genhtml -o $prj_dir/build/report $prj_dir/build/$prj.info
google-chrome $prj_dir/build/report/index.html
