#!/bin/bash

#docker run -it -v $PWD/home:/home/ debian-custom

grp="dsousa"

if [ "$grp" = "$LOGNAME" ];
then
	vol="/Volumes/pok"
	path=${PWD/$vol/'/Users'}
	docker run -it -v $path/home:/home/ debian-custom
else
	docker run -it -v $PWD/home:/home/ debian-custom
fi
