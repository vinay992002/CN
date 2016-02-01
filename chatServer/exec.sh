#!/bin/sh
rm c1
rm c2
rm c3
rm c4
rm cs
gcc server.c -o server
gcc client.c -o cl1 -lpthread
gcc client.c -o cl2 -lpthread
gcc client.c -o cl3 -lpthread
gcc client.c -o cl4 -lpthread