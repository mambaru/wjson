#!/bin/bash
script_name=$(realpath $0)
script_dir=$(dirname $script_name)
prj_dir=$(dirname $script_dir)

cp "$script_dir/suppressions.txt" "/tmp/suppressions.txt"
sed -i -- "s#\:\.\/#\:$prj_dir\/#g" "/tmp/suppressions.txt"

opt="--inconclusive --error-exitcode=1 --force --max-configs=128 \
--quiet --enable=all --suppress=missingIncludeSystem --suppressions-list=/tmp/suppressions.txt \
--template={id}:{file}:{line}:[{severity}]:{message} -I $prj_dir"

cppcheck $opt $@


