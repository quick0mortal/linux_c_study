#!/bin/bash
rm -f tcps
gcc tcps.c -o tcps
chmod +x tcps
./tcps 2333