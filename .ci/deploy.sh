#!/bin/bash

script_path=$(readlink -e $0)
script_dir=$(dirname $script_path)
project_name=$(basename $script_dir)

if [ -z "$1" ]; then
  prj="$project_name"
else
  prj="$1"
fi

dir="/tmp/$USER/.deploy/$prj"
arc="replaceme"
mkdir -m 0777 -p "$dir"

scp "gitlab-runner@repobuild2:/monamour/$prj/$arc" "$dir/"
tar -xf "$dir/$arc" -C "$dir"
rm "$dir/$arc"
sudo -u wwwrun cp $dir/* `pwd` || exit 1
rm -rf $dir
