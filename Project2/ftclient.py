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
from os import path
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


def get_dir(sock):
    data_size = sock.recv(4)
    data_size = unpack("I", data_size)
    received = str(sock.recv(data_size[0]), encoding="UTF-8").split("\x00")

    for val in received:
        print(val)


def send_message(sock, message):
    """Method used to send message to server.

    Arguments:
        s       -- This is the socket used for transmission.
    """
    to_send = bytes(message, encoding="UTF-8")
    sock.sendall(to_send)


def send_number(sock, message):
    to_send = pack('i', message)
    sock.send(to_send)


def receive_message(sock):
    """Method used to receive message from server.

    Arguments:
        s       -- Thi is the socket used for transmission.
    """
    data_size = sock.recv(4)
    data_size = unpack("I", data_size)
    print("The data size is: {}".format(data_size[0]))
    # received_msg = str(sock.recv(data_size[0]), encoding="UTF-8");
    return recvall(sock, data_size[0])


def recvall(sock, n):
    received = ""
    while len(received) < n:
        packet = str(sock.recv(n - len(received)), encoding="UTF-8")
        if not packet:
            return None
        received += packet
    return received


def make_request(conn, cmd, data):
    send_message(conn, cmd + "\0")
    send_number(conn, data)


def receive_file(conn, filename):
    buffer = receive_message(conn)
    if path.isfile(filename):
        filename = filename.split(".")[0] + "_copy.txt"

    with open(filename, 'w') as f:
        f.write(buffer)


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
    filename = ""
    print(len(sys.argv))

    if len(sys.argv) is 5:
        data_port = int(sys.argv[4])

    elif len(sys.argv) is 6:
        filename = sys.argv[4]
        data_port = int(sys.argv[5])

    if command not in ["-g", "-l"]:
        raise ValueError("The only commands accepted are -g or -l!")

    server = initiate_contact(host, port)
    make_request(server, command, data_port)

    if command == "-l":
        data = initiate_contact(host, data_port)
        print("Receiving directory structure from {}: {}".format(host, data_port))
        get_dir(data)
        data.close()

    if command == "-g":
        print("The length of the file {} is: {}".format(filename, len(filename)))
        send_number(server, len(filename))
        send_message(server, filename + "\0")

        result = receive_message(server)
        if result == "FILE NOT FOUND!":
            print("{}: {} says {}".format(host, port, result))
        elif result == "FOUND!":
            print("The file has been found")
            data = initiate_contact(host, data_port)
            print("Data conn started")
            receive_file(data, filename)
            data.close()

    server.close()

