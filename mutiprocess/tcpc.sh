#!/bin/bash
rm -f tcpc
gcc tcpc.c -o tcpc
chmod +x tcpc
./tcpc 127.0.0.1 2333