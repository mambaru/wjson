#!/bin/bash
script_name=$(readlink -e $0)
script_dir=$(dirname $script_name)
prj_dir=$(dirname $script_dir)

if [ -z "$1" ]; then
  project_dir=$prj_dir
else
  project_dir=$1
fi

if [ -z "$2" ]; then
  suppressions_txt="$project_dir/.ci/suppressions.txt"
else
  suppressions_txt=$2
fi

if [ -z "$3" ]; then
  exclude_txt="$project_dir/.ci/exclude_folders.txt"
else
  exclude_txt=$3
fi

if [ ! -f $suppressions_txt ]; then
    echo "ERROR: File $suppressions_txt not found!"
    exit 1
fi

project_dir=$(readlink -e $project_dir)
suppressions_txt=$(readlink -e $suppressions_txt)
suppressions_dir=$(dirname $suppressions_txt)
suppressions_lst="$suppressions_dir/suppressions.lst"

cp "$suppressions_txt" "$suppressions_lst"
sed -i -- "s#\:\.\/#\:$project_dir\/#g" "$suppressions_lst"


while read i; do
  exclude_args="$exclude_args -i $i"
done < $exclude_txt

opt="--inconclusive --error-exitcode=1 --force --max-configs=128 \
--quiet --enable=all --suppress=missingIncludeSystem --suppressions-list=$suppressions_lst \
--template={id}:{file}:{line}:[{severity}]:{message} -i $project_dir/external -i $project_dir/build $exclude_args" 

cppcheck $opt "${@:4}" $project_dir
res=$?

rm --force $suppressions_lst
exit $res
