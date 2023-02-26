#!/bin/bash

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root."
  exit
fi

docker run --rm \
  -v $SCRIPTPATH/../src:/src -w /src \
  --device /dev/ttyUSB0 \
  espressif/idf:release-v4.4 idf.py flash -p /dev/ttyUSB0
