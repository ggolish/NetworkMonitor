# netmon
A packet sniffer written as a project for graduate level network programming course at Indiana State University.

## Description
Opens a raw socket and monitors all network traffic for a given network device. Displays statistics such as packet types, counts, and data rate. Includes a real-time display of packets being received, and displays all MAC and IP addresses encountered.

## Instructions
After cloning the repository, simple run the command ``make netmon`` to build the project. Then run the ``netmon`` executable with root privileges according to the following scheme.
```
netmon [device-name]
```
`device-name` is a required parameter. It is the name of the network device to monitor, e.g. `eth0`. Note: names of devices can be determined with the command `ifconfig`.

## Purpose
This project is intended to be used to aid in the development of a custom high-speed file transfer protocol. More info on this will be available at a later date.

## Author
Written by Gage Golish, Fall 2018.
