# CS372 Project 2
Author: Rob Navarro
Date: 11/26/2016


I have provided a make file that compiles ftserver.c into an executable. To start the chat serve type the following into a terminal:

- make
- ./ftserver [port]

Now in a separate window we can start ftclient.py. This was written in python 3 and can be started on flip by typing the following into a terminal:

- python3 ftclient.py [hostname] [port number] [command] [filename] [data port]

I performed my testing on flip1 and flip so the host I used was: flip1.engr.oregonstate.ed u and flip2.engr.oregonstate.edu I also found port 50000 and 50001 to be successful for testing purposes.

The ftclient will exit after the file transfer is complete. The ftserver can be quit by using ctrl-c.

If you need to remove the executable and any copy files made you can use the follow command from make:

- make clean
