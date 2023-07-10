#!/bin/bash

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

export TERM=linux
export TERMINFO=/etc/terminfo
docker run --rm -v $SCRIPTPATH/../src:/src -w /src -it espressif/idf:release-v4.4 idf.py menuconfig