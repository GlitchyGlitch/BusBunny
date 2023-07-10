#!/bin/bash

SCRIPT=$(realpath $0)
SCRIPTPATH=$(dirname $SCRIPT)

docker run --rm \
  -v $SCRIPTPATH/../src:/src -w /src \
  espressif/idf:release-v4.4 idf.py build
