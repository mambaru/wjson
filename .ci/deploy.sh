#!/bin/bash

script_path=$(readlink -e $0)
script_dir=$(dirname $script_path)
project_name=$(basename $script_dir)

if [ -z "$1" ]; then
  prj="$project_name"
else
  prj="$1"
fi

log=/log/${prj}.log
dname="${prj}d"

backup=$(date +%F-%X)
sudo -u wwwrun mkdir $backup

sudo -u wwwrun shopt -s extglob dotglob
sudo -u wwwrun mv !($backup) $backup
sudo -u wwwrun rm -rf $PWD
sudo -u wwwrun shopt -u dotglob

dir="/tmp/$USER/.deploy/$prj"
arc="replaceme"
mkdir -m 0777 -p "$dir"

scp "gitlab-runner@repobuild2:/monamour/$prj/$arc" "$dir/" || exit 1
tar -xf "$dir/$arc" -C "$dir" || exit 1
rm "$dir/$arc"
sudo -u wwwrun cp $dir/* $PWD || exit 1
rm -rf $dir

echo "Перезапуск $dname"
while killall -q $dname; do
  sleep 1;
  tail $log
done
$PWD/${dname}.sh
tail -f $log
