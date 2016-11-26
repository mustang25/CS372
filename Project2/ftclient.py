#!/usr/bin/env python3
"""This program is for the chat client. It is used so that a client can connect to the chat server.
My main refernce for this program was: https://docs.python.org/3/library/socket.html
https://docs.python.org/3.5/library/struct.html
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
from struct import *


def initiate_contact(host, port):
    """Method used to initiate contact with server.

    Arguments:
        HOST    -- This is the hostname for the server.
        PORT    -- This is the port that the chatserver is running on.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    return s


def start_datasocket(host, port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen(1)

        conn, addr = s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                print("In conn...")
                data_size = conn.recv(4)
                data_size = unpack("I", data_size)
                received = str(conn.recv(data_size[0]), encoding="UTF-8").split("\x00")
                conn.close()
                if received is not None:
                    break
        return received
    # print(host)
    # print("starting data socket!")
    # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # print("socket created")
    # s.bind((host, port))
    # print("starting to listen!")
    # print(host)
    # s.listen(10)
    # while 1:
    #     conn, addr = s.accept()
    #     with conn:
    #         print("In conn...")
    #         data_size = conn.recv(4)
    #         data_size = unpack("I", data_size)
    #         received = str(conn.recv(data_size[0]), encoding="UTF-8").split("\x00")
    #         conn.close()
    #         if received is not None:
    #             break
    #     return received




def send_message(sock, message):
    """Method used to send message to server.

    Arguments:
        s       -- This is the socket used for transmission.
    """
    to_send = bytes(message, encoding="UTF-8")
    sock.sendall(to_send)


def send_port(sock, message):
    to_send = pack('i', message)
    sock.send(to_send)


def receive_message(sock):
    """Method used to receive message from server.

    Arguments:
        s       -- Thi is the socket used for transmission.
    """
    quit_sending = False
    received_message = str(sock.recv(500), encoding="UTF-8")

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
    arg_length = len(sys.argv)
    if arg_length < 5 or arg_length > 6:
        print("Usage: python3 ftclient.py [hostname] [port number] [command] [filename] [data port]")
        exit(1)

    host = sys.argv[1]
    port = int(sys.argv[2])
    command = sys.argv[3]
    data_port = 0

    if len(sys.argv) is 6:
        data_port = int(sys.argv[5])

    elif len(sys.argv) is 7:
        filename = sys.argv[5]
        data_port = int(sys.argv[6])

    if command not in ["-g", "-l"]:
        raise ValueError("The only commands accepted are -g or -l!")

    server = initiate_contact(host, port)
    send_message(server, command + "\0")
    send_port(server, data_port)

    if command == "-l":
        directory = start_datasocket("127.0.0.1", data_port)
        print("Receiving directory structure from {}: {}".format(host, data_port))
        for var in directory:
            print(var)

    server.close()




