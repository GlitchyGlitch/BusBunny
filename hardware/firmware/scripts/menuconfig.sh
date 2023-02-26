#!/bin/bash

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root."
  exit
fi
export TERM=linux
export TERMINFO=/etc/terminfo
docker run --rm -v $SCRIPTPATH/../src:/src -w /src -it espressif/idf:release-v4.4 idf.py menuconfig