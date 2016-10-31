import socket
import sys

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

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while chat_quit is False:
        print("{}> ".format(handler), end="")
        user_input = input()
        to_send = "{}> ".format(handler) + user_input + "\n"
        if user_input == "\quit":
            chat_quit = True
        to_send = bytes(to_send, encoding="UTF-8")
        s.sendall(to_send)

        if chat_quit is False:
            received_message = str(s.recv(500), encoding="UTF-8")

            if received_message == "\\quit\n":
                print("The server ended the chat session, exiting.")
                chat_quit = True
            else:
                print("Chat_Server> {}".format(received_message), end="")




