all:
	gcc -Wall -o minIP minIP.c -DLINUX
clean:
	rm -f minIP
