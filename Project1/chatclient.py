#!/usr/bin/env python3

import socket
import sys


def initiate_contact(HOST, PORT):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    return s


def send_message(s):
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
    quit_sending = False
    received_message = str(s.recv(500), encoding="UTF-8")

    if received_message == "\\quit\n":
        print("The server ended the chat session, exiting.")
        quit_sending = True
    else:
        print("Chat_Server> {}".format(received_message), end="")

    return quit_sending


if __name__ == '__main__':

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




