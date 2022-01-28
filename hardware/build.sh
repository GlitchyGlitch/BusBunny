#!/bin/bash

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root."
  exit
fi

docker run --rm \
  -v $SCRIPTPATH/firmware/blackbox:/blackbox -w /blackbox \
  espressif/idf idf.py build

chown -R $SUDO_USER:$SUDO_USER $SCRIPTPATH/firmware/blackbox
chmod -R +700 $SCRIPTPATH/firmware/blackbox
# docker run --rm \
#   -v $PWD/firmware/blackbox:/blackbox -w /blackbox \
#   --device /dev/ttyUSB0 \
#   espressif/idf idf.py flash -p /dev/ttyUSB0
