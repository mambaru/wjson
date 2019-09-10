#!/bin/bash

script_path=$(readlink -e $0)
script_dir=$(dirname $script_path)
project_dir=$(dirname $script_dir)
project_name=$(basename $project_dir)
build_dir="$project_dir/build"

if [ -z "$1" ]; then
  cov_report=$build_dir/cov-report
else
  cov_report="$1"
fi

if [ ! -d "$build_dir" ]; then
  echo "You need to build a project with the option -DCODE_COVERAGE=ON and run tests"
  exit 1
fi

# Проверяем, что созданы необходимые файлы
gcda_count="$(find ./ -type f -iname '*.gcda' | wc -l)"

if [ "$gcda_count" = "0" ]; then
  echo "Not found * .gcda files"
  echo "You need to build a project with the option -DCODE_COVERAGE=ON and run tests"
  exit 1
fi

cov_info=$build_dir/$project_name-coverage.info

rm -f $cov_info

echo "We collect data for the report..."
lcov --quiet --capture --directory $build_dir --base-directory $project_dir --no-external --output-file $cov_info || exit 1
echo "Delete data for submodules..."
lcov --quiet --remove $cov_info "`pwd`/external/*" --output-file $cov_info || exit 2

if [ "$cov_report" != "--" ]; then
  rm -rf $cov_report
  echo "Create html report..."
  mkdir -p $cov_report
  genhtml --quiet -o $cov_report $cov_info || exit 3

  lcov --summary $cov_info
  echo "To view the report run:"
  echo -e "\tgoogle-chrome $cov_report/index.html"
fi
