minIP
========

A minimalist IP stack written in ANSI C.

This needs to be run on a Linux machine with root access - ideally on a network interface that does not have an IP address already associated with it.

Eventually it will also run on top of the BareMetal exokernel.

Goals
--------

Provide enough of a stack to server a simple static webpage.

Usage
--------

	./minIP eth1 192.168.0.99 255.255.255.0

Why
--------

If this can be done in Phython (see [teeceepee](https://github.com/jvns/teeceepee) and its [blog post](http://jvns.ca/blog/2014/08/12/what-happens-if-you-write-a-tcp-stack-in-python/)), then it can be done in C!

What works
--------

Protocol | Functional 
-----|-----
ARP | yes
IP | sort of
ICMP | yes
TCP | no
UDP | no
TCPv6 | no
UDPv6 | no