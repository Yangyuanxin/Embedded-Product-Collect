#!/usr/bin/env python
import socket 
socket.setdefaulttimeout(5)
c = socket.socket()
c.connect(('agnss.techtotop.com',9011))
c.send('all')
temp = ''
data = ''
while True:
  temp = c.recv(1024)
  if not len(temp):
    break
  else:
    data += temp

#f=open('data','wb')
#print >>f,data
#f.close()
import serial
tty = serial.Serial()
tty.port = 'COM1'
tty.baudrate = 9600
tty.open()
tty.write(data)
tty.close()
