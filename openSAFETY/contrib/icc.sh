#!/bin/bash

source /opt/intel/bin/compilervars.sh $1 
if [ ! $? -eq 0 ] ; then
    exit 1
fi

if [ -d buildicc-$1 ] ; then
    rm -rf buildicc-$1 
fi

mkdir buildicc-$1
cd buildicc-$1
CC=$(which icc) CXX=$(which icpc) cmake ../src
