# netmon
A packet sniffer written as a project for graduate level network programming course at Indiana State University.

## Description
Opens a raw socket and monitors all network traffic for a given network device. Displays statistics such as packet types, counts, and data rate. Includes a real-time display of packets being received, and displays all MAC and IP addresses encountered.

## Instructions
After cloning the repository, simple run the command ``make netmon`` to build the project. Then run the ``netmon`` executable with root privileges according to the following scheme.
```
netmon [-d <device-name>] [-t <ethertype>]
```
- ``device-name`` is the name of the desired network device to be monitored. The default value is ``eth0``.
- ``ethertype`` is a specific ethernet type to monitor. This value can be a hexadecimal string, ``arp``, ``ip4``, ``ip6``, or ``netrans``.

## Purpose
This project is intended to be used to aid in the development of a custom high-speed file transfer protocol. More info on this will be available at a later date.

## Author
Written by Gage Golish, Fall 2018.
