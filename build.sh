#!/bin/sh
rm libBareMetal.c
rm libBareMetal.h
if [ -x "$(command -v curl)" ]; then
	curl -s -o libBareMetal.c https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.c
	curl -s -o libBareMetal.h https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.h
else
	wget -q https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.c
	wget -q https://raw.githubusercontent.com/ReturnInfinity/BareMetal/master/api/libBareMetal.h
fi
gcc -c -m64 -Wall -W -pedantic -std=c99 -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o minIP.o minIP.c -DBAREMETAL
gcc -c -m64 -Wall -W -pedantic -fno-builtin -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o libBareMetal.o libBareMetal.c
objcopy --remove-section .comment minIP.o
objcopy --remove-section .eh_frame minIP.o
objcopy --remove-section .comment libBareMetal.o
objcopy --remove-section .eh_frame libBareMetal.o
ld -T app.ld -o minIP.app minIP.o libBareMetal.o
