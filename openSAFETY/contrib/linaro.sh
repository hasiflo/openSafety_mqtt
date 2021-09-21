#!/bin/bash

LINARO_PATH=/opt/linaro

export PATH=$LINARO_PATH/bin:$PATH
export LD_LIBRARY_PATH=$LINARO_PATH/lib64:$LINARO_PATH/lib:$LD_LIBRARY_PATH

if [ ! $? -eq 0 ] ; then
    exit 1
fi

if [ -d buildlinaro ] ; then
    rm -rf buildlinaro 
fi

mkdir buildlinaro
cd buildlinaro
CC=$(which gcc) CXX=$(which g++) cmake ../src
