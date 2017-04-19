#!/bin/sh
gcc -c -m64 -Wall -W -pedantic -std=c99 -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o minIP.o minIP.c -DBAREMETAL_STANDALONE
gcc -c -m64 -Wall -W -pedantic -nostdlib -nostartfiles -nodefaultlibs -fomit-frame-pointer -mno-red-zone -o libBareMetal.o libBareMetal.c
objcopy --remove-section .comment minIP.o
objcopy --remove-section .eh_frame minIP.o
objcopy --remove-section .comment libBareMetal.o
objcopy --remove-section .eh_frame libBareMetal.o
ld -T app.ld -o minIP.app minIP.o libBareMetal.o
