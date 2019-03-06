#!/bin/sh
if test -z "$1"
then
    mkdir -p atmosphere/titles/010000000000100C/flags
    touch atmosphere/titles/010000000000100C/flags/fsmitm.flag
    cp -R romfs atmosphere/titles/010000000000100C/
    cp layoff.nsp atmosphere/titles/010000000000100C/exefs.nsp
else
    mkdir -p $1/atmosphere/titles/010000000000100C
    cp layoff.nsp $1/atmosphere/titles/010000000000100C/exefs.nsp
fi