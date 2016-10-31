# CS372 Project 1
Author: Rob Navarro
Date: 10/30/2016

Due to misreading the assignment description, I accidentally switched around the programming languages. I did talk with Bassem during his office hours on 10/30 and he told me that I would not be counted off for the errors. Because of this I have the following files.

- chatserve.c
- chatclient.py

I have provided a make file that compiles chatserve.c into an executable. To start the chat serve type the following into a terminal:

- make
- ./chatserve [port]

Now in a separate window we can start chatclient.py. This was written in python 3 and can be started on flip by typing the following into a terminal:

- python3 chatclient.py [host][port]

I performed my testing on flip1 so the host I used was: flip1.engr.oregonstate.edu. I also found port 50000 to be successful for testing purposes.

In order to quit you can send '\quit' from the server or client.

If you need to remove the executable you can use the follow command from make:

- make clean
