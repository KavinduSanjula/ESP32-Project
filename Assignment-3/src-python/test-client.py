import socket

server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

host = '192.168.43.236'
port = 4444


server.connect((host,port))