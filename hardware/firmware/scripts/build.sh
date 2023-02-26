#!/bin/bash

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root."
  exit
fi
# rm -rf $SCRIPTPATH/blackbox/build


docker run --rm \
  -v $SCRIPTPATH/blackbox:/blackbox -w /blackbox \
  espressif/idf:release-v4.4 idf.py build

# chown -R $SUDO_USER:$SUDO_USER $SCRIPTPATH/blackbox
# chmod -R +700 $SCRIPTPATH/blackbox
docker run --rm \
  -v $SCRIPTPATH/blackbox:/blackbox -w /blackbox \
  --device /dev/ttyUSB0 \
  espressif/idf:release-v4.4 idf.py flash -p /dev/ttyUSB0

# docker run --rm \
#   -v $SCRIPTPATH/blackbox:/blackbox -w /blackbox \
#   espressif/idf:release-v4.4 idf.py menuconfig

