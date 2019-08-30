#!/bin/bash

md5val=$(echo $PWD | md5sum | cut -f1 -d" ")
export report_wip="/tmp/need-upgrade-$md5val-wip.txt"
export report_rel="/tmp/need-upgrade-$md5val-rel.txt"

rm -f "$report_wip"
rm -f "$report_rel"


function need_upgrade ()
{
  git fetch
  prjname=${PWD##*/}
  if [ "$prjname" == "cmake-ci" ] || [ "$prjname" == "faslib" ]; then
    return
  fi

  count=$(git rev-list --count origin/pre-release ^origin/master)
  if (( $count!=0 )); then
    msg=$(git log origin/pre-release -1 --pretty=%B)
    if [[ $msg == *"Night Build"* ]]; then
      echo -e "\t\t$prjname: $count" >> $report_rel
    else
      count=$(git rev-list --count origin/wip-devel ^origin/master)
      if (( $count!=0 )); then
        msg=$(git log origin/wip-devel -1 --pretty=%B)
        if [[ $wipmsg == *"Night Build"* ]]; then
          echo "\t\t$prjname: $count коммитов" >> $report_wip
        fi
      fi
    fi
  fi
}

#git submodule sync
#git submodule update --init
export -f need_upgrade
git submodule foreach need_upgrade

ret=0
if [ -s "$report_wip" ] || [ -s "$report_rel" ]; then
  echo ""
  echo "********************************************************************"
  echo "* Не прошла проверка допустимости ночного автоматического апгрейда.*"
  echo "********************************************************************"
  echo ""
  echo "Следующие модули имеют запросы на слияние:"
  
  if [ -s "$report_wip" ]; then
    echo -e "\tДля wip-devel:"
    cat "$report_wip"
    rm "$report_wip"
    ret=1
  fi
  
  if [ -s "$report_rel" ]; then
    echo -e "\tДля pre-release:"
    cat "$report_rel"
    rm "$report_rel"
    ret=2
  fi
else
  echo "********************************************************************"
  echo "* Прошла проверка допустимости ночного автоматического апгрейда.   *"
  echo "********************************************************************"
fi

exit $ret
