#!/bin/bash
# дефолтный скрипт для запуска демона копируется с переименованием при деплое 

script_path=$(readlink -e $0)
script_dir=$(dirname $script_path)
project_name=$(basename $script_dir)
project_path=$script_dir/$project_name

if [ ! -f "$project_path" ]; then
  echo "Имя запускаемого бинарника должно совпадать с именем директории"
  exit 1
fi

if [ -z $1 ]; then
  if [[ "script_dir" == "$PWD" ]]; then
    conf="${project_name}.conf"
  else
    conf="${project_path}.conf"
  fi
  
  if [ ! -f $conf ]; then
    echo "ERROR: Не найден файл конфигурации $conf"
    echo "Можно задать другой файл первым параметром"
    exit
  fi

else
  conf="$1"
fi

export LD_LIBRARY_PATH="$script_dir/lib"

sudo $project_path -C $conf -u wwwrun -d -a 60 --pid-dir=/logs/ ${@:2} 
