#!/usr/bin/env python3
"""This program is for the server client. It is used so that a client can connect to the file transfer server.
My main reference for this program was: https://docs.python.org/3/library/socket.html
https://docs.python.org/3.5/library/struct.html
Program: ftclient.py
Author: Rob Navarro
Class: CS372

The python socket module is used to handle communication.

Methods:
initiate_contact(host, port):
    Method used to initiate contact with server.

    Arguments:
        host    -- This is the hostname for the server.
        port    -- This is the port that the ftserver is running on.

get_dir(sock):
    Method to get contents of server directory.
    Arguments:
        sock        -- This is the socket used for transmission.

send_message(sock, message):
    Method used to send message to server.

    Arguments:
        sock        -- This is the socket used for transmission.
        message     -- This is the message to be sent.

send_number(sock, message):
    Method to pack and send integer value.

    Arguments:
        sock        -- This is the socket used for transmission.
        message     -- This is the message to be sent.

receive_message(sock):
    Method used to receive message from server.

    Arguments:
        sock       -- Thi is the socket used for transmission.

ecvall(sock, n):
    Helper method used to get entire file transmission.

    Arguments:
        sock        -- This is the socket used for transmission.
        n           -- This is the number of bytes being transmitted.

make_request(conn, cmd, data):
    Method to make initial request to server.

    Arguments:
        conn        -- This is the socket used for transmission.
        cmd         -- This is the command to be sent to the server.
        data        -- This is the port for data to be transmitted on.

receive_file(conn, filename):
    Method to receive file

    Arguments:
        conn        -- This is the socket used for transmission.
        filename    -- This is the file to be transmitted.
"""
import socket
import sys
from os import path
from struct import *
from time import sleep


def initiate_contact(host, port):
    """Method used to initiate contact with server.

    Arguments:
        host    -- This is the hostname for the server.
        port    -- This is the port that the ftserver is running on.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    return s


def get_dir(sock):
    """Method to get contents of server directory.
    Arguments:
        sock        -- This is the socket used for transmission.
    """
    data_size = sock.recv(4)
    data_size = unpack("I", data_size)
    received = str(sock.recv(data_size[0]), encoding="UTF-8").split("\x00")

    for val in received:
        print(val)


def send_message(sock, message):
    """Method used to send message to server.

    Arguments:
        sock        -- This is the socket used for transmission.
        message     -- This is the message to be sent.
    """
    to_send = bytes(message, encoding="UTF-8")
    sock.sendall(to_send)


def send_number(sock, message):
    """Method to pack and send integer value.

    Arguments:
        sock        -- This is the socket used for transmission.
        message     -- This is the message to be sent.
    """
    to_send = pack('i', message)
    sock.send(to_send)


def receive_message(sock):
    """Method used to receive message from server.

    Arguments:
        sock       -- Thi is the socket used for transmission.
    """
    data_size = sock.recv(4)
    data_size = unpack("I", data_size)
    # received_msg = str(sock.recv(data_size[0]), encoding="UTF-8");
    return recvall(sock, data_size[0])


def recvall(sock, n):
    """Helper method used to get entire file transmission.

    Arguments:
        sock        -- This is the socket used for transmission.
        n           -- This is the number of bytes being transmitted.
    """
    received = ""
    while len(received) < n:
        packet = str(sock.recv(n - len(received)), encoding="UTF-8")
        if not packet:
            return None
        received += packet
    return received


def make_request(conn, cmd, data):
    """Method to make initial request to server.

    Arguments:
        conn        -- This is the socket used for transmission.
        cmd         -- This is the command to be sent to the server.
        data        -- This is the port for data to be transmitted on.
    """
    send_message(conn, cmd + "\0")
    send_number(conn, data)


def receive_file(conn, filename):
    """Method to receive file

    Arguments:
        conn        -- This is the socket used for transmission.
        filename    -- This is the file to be transmitted.
    """
    buffer = receive_message(conn)
    if path.isfile(filename):
        filename = filename.split(".")[0] + "_copy.txt"

    with open(filename, 'w') as f:
        f.write(buffer)


if __name__ == '__main__':
    """This is the main driver for the server client.

    It confirms that the proper input was provided by the user. If the proper input was not provided the program
    exits and displays usage.

    Once input has been confirmed contact is initiated with the server.

    Depending on the command, data is received from the client.
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

    if len(sys.argv) is 5:
        data_port = int(sys.argv[4])

    elif len(sys.argv) is 6:
        filename = sys.argv[4]
        data_port = int(sys.argv[5])

    if command not in ["-g", "-l"]:
        raise ValueError("The only commands accepted are -g or -l!")

    if port < 1024 or port > 65535 or data_port < 1024 or data_port > 65525:
        raise ValueError("Invalid port value, port must be between 1024 and 65535!")

    # Start server socket and make initial request
    server = initiate_contact(host, port)
    make_request(server, command, data_port)

    # If the command is l, wait for the server to transmit the directory contents on the data port.
    if command == "-l":
        sleep(1)
        data = initiate_contact(host, data_port)
        print("Receiving directory structure from {}: {}".format(host, data_port))
        get_dir(data)
        data.close()

    # If the command is g, wait for the server to transmit the file. The file is then saved to the local machine.
    if command == "-g":
        send_number(server, len(filename))
        send_message(server, filename + "\0")

        result = receive_message(server)
        if result == "FILE NOT FOUND!":
            print("{}: {} says {}".format(host, port, result))
        elif result == "FOUND!":
            print("Receiving \"{}\" from {}: {}".format(filename, host, data_port))
            sleep(1)
            data = initiate_contact(host, data_port)
            receive_file(data, filename)
            print("File transfer complete!")
            data.close()

    server.close()

