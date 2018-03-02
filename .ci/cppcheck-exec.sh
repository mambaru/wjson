#!/bin/bash
script_name=$(readlink -e $0)
script_dir=$(dirname $script_name)
prj_dir=$(dirname $script_dir)

cp "$script_dir/suppressions.txt" "$script_dir/suppressions.lst"
sed -i -- "s#\:\.\/#\:$prj_dir\/#g" "$script_dir/suppressions.lst"

opt="--inconclusive --error-exitcode=1 --force --max-configs=128 \
--quiet --enable=all --suppress=missingIncludeSystem --suppressions-list=$script_dir/suppressions.lst \
--template={id}:{file}:{line}:[{severity}]:{message} -I $prj_dir"

cppcheck $opt $@
res=$?

rm --force $script_dir/suppressions.lst
exit $res
