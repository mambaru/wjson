#!/bin/bash

scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
prjdir=${PWD}
prjname="$(basename ${prjdir})"

echo "Это пользовательский скрипт сбросом (reset.sh)"
