#!/bin/bash
rm -f tcpser
gcc tcpser.c -o tcpser
chmod +x tcpser
./tcpser 2333