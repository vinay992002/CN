First of all you need to install rpcgen. Type the following command in terminal.

sudo apt-get install rpcbind

after installation type 

rpcinfo

To check if rpcbind installed properly or not.

Every RPC prog. folder contains ".x" file.

execute that ".x" file using rpcgen with the following command.

rpcgen -a -C filename.x

It will generate 10 files. now we have to change just two files. client and server file according to our use.

after changing type the following command to compile the program.

make -f Makefile.<filename> 

example for square prog.

make -f Makefile.square    

to run the prog.

type 
sudo ./servername

ex. for square program type

sudo ./square_server

and for client type

sudo ./square_client localhost


