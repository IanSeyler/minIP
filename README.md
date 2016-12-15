minIP
========

A minimalist IP stack written in ANSI C.

This needs to be run on a Linux machine with root access - ideally on a network interface that does not have an IP address already associated with it.

This also runs on top of the BareMetal exokernel.

Goals
--------

Provide enough of a stack to serve a simple static webpage.

Usage
--------

Linux:

	./minIP eth1 192.168.0.99 255.255.255.0 192.168.0.1

BareMetal:

	minIP.app en0 192.168.0.99 255.255.255.0 192.168.0.1

Why
--------

If this can be done in Phython (see [teeceepee](https://github.com/jvns/teeceepee) and its [blog post](http://jvns.ca/blog/2014/08/12/what-happens-if-you-write-a-tcp-stack-in-python/)), then it can be done in C!

Also, its a great proof of concept to learn the fundamentals in preparation for an x86-64 assembly re-write I plan on doing in the future.

What works
--------

Protocol | Functional
-----|-----
ARP | yes
IP | partially
ICMP | yes
TCP | partially
UDP | no
TCPv6 | no
UDPv6 | no

Todo
--------
- Resending dropped packets
- Large packet support
- Handling more than one incoming connection at once
