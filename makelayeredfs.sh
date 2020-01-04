#!/bin/sh
if test -z "$1"
then
    mkdir -p atmosphere/contents/010000000000100C/flags
    touch atmosphere/contents/010000000000100C/flags/fsmitm.flag
    cp -R romfs atmosphere/contents/010000000000100C/
    cp layoff.nsp atmosphere/contents/010000000000100C/exefs.nsp
else
    mkdir -p $1/atmosphere/contents/010000000000100C
    cp layoff.nsp $1/atmosphere/contents/010000000000100C/exefs.nsp
fi
