#!/bin/sh
if test -z "$1"
then
    mkdir -p atmosphere/contents/010000000000100C/flags
    touch atmosphere/contents/010000000000100C/flags/fsmitm.flag
    cp -R romfs atmosphere/contents/010000000000100C/
    cp layoff.nsp atmosphere/contents/010000000000100C/exefs.nsp
    mkdir -p atmosphere/contents/01006C61796F6666/flags
	cp service/service.nsp atmosphere/contents/01006C61796F6666/exefs.nsp
	touch atmosphere/contents/01006C61796F6666/flags/boot2.flag
else
    mkdir -p $1/atmosphere/contents/010000000000100C
    touch $1/atmosphere/contents/010000000000100C/flags/fsmitm.flag
    cp layoff.nsp $1/atmosphere/contents/010000000000100C/exefs.nsp
    mkdir -p $1/atmosphere/contents/01006C61796F6666/flags
	cp service/service.nsp $1/atmosphere/contents/01006C61796F6666/exefs.nsp
	touch $1/atmosphere/contents/01006C61796F6666/flags/boot2.flag
fi
