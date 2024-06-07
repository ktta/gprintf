#!/bin/sh
# A simple script for looping over things within gp.c

grep -e '^VALS=' -e '^DOIT=' gp.c | tail -n 2 > /tmp/kk

. /tmp/kk

# printf is used so that this file can be embedded into the main source..

printf "%c%c%c%c %c %c%c%c%c" / '*' '*' '*' '{' '*' '*' '*' '/'   > /tmp/xx
echo >> /tmp/xx
for X in $VALS; do
   echo "$DOIT" | sed -e "s?@?$X?g"   >> /tmp/xx
done
printf "%c%c%c%c %c %c%c%c%c" / '*' '*' '*' '}' '*' '*' '*' '/'   >> /tmp/xx
echo >> /tmp/xx
