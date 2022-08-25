from ast import arg
from concurrent.futures import thread
import socket
import threading
from time import sleep

def spam(socket):
    while 1:
        socket.send(b"Spamming")

for i in range (0, 100):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", 9999))
    x = threading.Thread(target=spam, args=(s,))
    x.start()