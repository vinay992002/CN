how to compile and run the program
 every terminal should be in sudo mode. i.e. 
 type sudo su in every terminal u open

1 gcc SM.c 
2 gcc platfoem.c -o pl -lpthread
3 gcc tv.c -o tv
4 gcc train.c -o tr
5 ./pl 0   (platform 0 starts by this)
6 ./pl 1   (platform 1 starts by this)
7 ./pl 2   (platform 2 starts by this)
8 ./a.out   (statiom master starts with this)
9  ./tv    (starting tv adds)
10 ./tr <port no> (port no. should be one of those displayed on terminal of station master)

