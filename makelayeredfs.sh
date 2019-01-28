#!/bin/sh
if test -z "$1"
then
    mkdir atmosphere
    mkdir atmosphere/titles
    mkdir atmosphere/titles/010000000000100C
    mkdir atmosphere/titles/010000000000100C/exefs/
    npdmtool npdm.json atmosphere/titles/010000000000100C/exefs/main.npdm
    cp build/exefs/main atmosphere/titles/010000000000100C/exefs/main
    touch atmosphere/titles/010000000000100C/exefs/rtld.stub
else
    mkdir $1/atmosphere
    mkdir $1/atmosphere/titles
    mkdir $1/atmosphere/titles/010000000000100C
    mkdir $1/atmosphere/titles/010000000000100C/exefs/
    npdmtool npdm.json $1/atmosphere/titles/010000000000100C/exefs/main.npdm
    cp build/exefs/main $1/atmosphere/titles/010000000000100C/exefs/main
    touch $1/atmosphere/titles/010000000000100C/exefs/rtld.stub
fi