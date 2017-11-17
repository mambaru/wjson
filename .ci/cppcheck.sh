#!/bin/bash
script_name=$(realpath $0)
script_dir=$(dirname $script_name)
prj_dir=$(dirname $script_dir)

echo "$prj_dir"
$script_dir/cppcheck-exec.sh $@ $prj_dir |& tee /tmp/cppcheck.cppcheck 
sh -c "! grep '\[' /tmp/cppcheck.cppcheck"
exit $?

