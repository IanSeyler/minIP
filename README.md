minIP
========

A minimalist IP stack written in ANSI C.

This needs to be run on a Linux machine with root access - ideally on a network interface that does not have an IP address already associated with it.

Usage
--------

	./minIP eth1 192.168.0.99 255.255.255.0

Why
--------

If this can be done in Phython (see [teeceepee](https://github.com/jvns/teeceepee) and its [blog post](http://jvns.ca/blog/2014/08/12/what-happens-if-you-write-a-tcp-stack-in-python/)), then it can be done in C!