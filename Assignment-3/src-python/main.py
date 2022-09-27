from os import write
import socket

FILENAME = 'data.txt'

server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

host = '192.168.43.236'
port = 4444

server.bind((host,port))
server.listen(1)

def write_to_file(time,temp,light):
    file = open(FILENAME,'a')
    file.write(f'Time:{time}, Temperature:{temp}, LightLevel:{light}\n')
    file.close()

while True:
    client, addr = server.accept()

    if client:
        print('client connected!',addr)

        dataset = client.recv(1024).decode('utf-8')
        hour,min,sec,temp,light = dataset.split(',')
        time = f'{hour}:{min}:{sec}'
        write_to_file(time,temp,light)