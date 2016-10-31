#!/usr/bin/env python3
"""This program is for the chat client. It is used so that a client can connect to the chat server.
My main refernce for this program was: https://docs.python.org/3/library/socket.html

Program: chatclient.py
Author: Rob Navarro
Class: CS372

The python socket module is used to handle most of the communication.

Methods:
    initiate_contact(HOST, PORT):
    Method used to initiate contact with server.

    Arguments:
        HOST    -- This is the hostname for the server.
        PORT    -- This is the port that the chatserver is running on.

    send_message(s)
    Method used to send message to server.

    Arguments:
        s       -- This is the socket used for transmission.

    receive_message(s)
    Method used to receive message from server.

    Arguments:
        s       -- Thi is the socket used for transmission.
"""
import socket
import sys


def initiate_contact(HOST, PORT):
    """Method used to initiate contact with server.

    Arguments:
        HOST    -- This is the hostname for the server.
        PORT    -- This is the port that the chatserver is running on.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    return s


def send_message(s):
    """Method used to send message to server.

    Arguments:
        s       -- This is the socket used for transmission.
    """
    quit_sending = False
    print("{}> ".format(handler), end="")
    user_input = input()
    to_send = "{}> ".format(handler) + user_input + "\n"
    if user_input == "\quit":
        quit_sending = True
    to_send = bytes(to_send, encoding="UTF-8")
    s.sendall(to_send)

    return quit_sending


def receive_message(s):
    """Method used to receive message from server.

    Arguments:
        s       -- Thi is the socket used for transmission.
    """
    quit_sending = False
    received_message = str(s.recv(500), encoding="UTF-8")

    if received_message == "\\quit\n":
        print("The server ended the chat session, exiting.")
        quit_sending = True
    else:
        print("Chat_Server> {}".format(received_message), end="")

    return quit_sending


if __name__ == '__main__':
    """This is the main driver for the chat client.

    It confirms that the proper input was provided by the user. If the proper input was not provided the program
    exits. The user is also prompted for a handler of 1 - 10 characters.

    Once input has been confirmed contact is initiated with the server.

    A while loop then continues until the user send \quit or the server sends \quit.
    """
    if len(sys.argv) != 3:
        print("Usage: python3 chatclient.py [hostname] [port number]")
        exit(1)

    if len(sys.argv) != 3:
        print("Usage: python3 chatclient.py [hostname] [port number]")
        exit(1)

    HOST = sys.argv[1]
    PORT = int(sys.argv[2])
    chat_quit = False

    print("Enter a handler for chat (handler limit is 10 characters)", end=": ")
    handler = input()

    if len(handler) < 1 or len(handler) > 10:
        print("Invalid handler, handler must be between 1 - 10 characters.")
        exit(1)

    server = initiate_contact(HOST, PORT)

    while chat_quit is False:
        chat_quit = send_message(server)
        if chat_quit is False:
            chat_quit = receive_message(server)
