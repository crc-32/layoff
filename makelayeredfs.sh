#!/bin/sh
if test -z "$1"
then
    mkdir atmosphere
    mkdir atmosphere/titles
    mkdir atmosphere/titles/010000000000100C
    cp layoff.nsp atmosphere/titles/010000000000100C/exefs.nsp
else
    mkdir $1/atmosphere
    mkdir $1/atmosphere/titles
    mkdir $1/atmosphere/titles/010000000000100C
    cp layoff.nsp $1/atmosphere/titles/010000000000100C/exefs.nsp
fi