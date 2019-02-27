#!/bin/bash

iptables -t filter -D INPUT -p tcp --dport 22 -j ACCEPT

iptables -t filter -D OUTPUT -p icmp -j DROP

iptables -t filter -A INPUT -p icmp -j ACCEPT


