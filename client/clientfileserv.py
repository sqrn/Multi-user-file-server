#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
from socket import *
from threading import Thread
import time
from types import *

class ClientFileServer(Thread):

    def __init__(self, port=9999, path=None):
        Thread.__init__(self)
        self.path = '/home/mariusz/workspace/cpp/projekt_pap/WUP/client/myfiles/'
        self.alive = True
        self.port = port

    def run(self):
        try:
            self.s = socket(AF_INET,SOCK_STREAM)
            self.s.bind(('',self.port))
            self.s.listen(1)
            self.listen_sock()
        except error, (value,message):
            if self.s:
                self.s.close()
            print "Nie można utworzyć połączenia dla serwera plików na porcie %s" % port
            sys.exit(0)

    def listen_sock(self):
        while(self.alive):
            client,addr = self.s.accept() #zwraca deskryptor polaczenia i adres polaczonego klienta
            #print "\nPołączony klient - ", addr
            # przesylanie pliku do klienta
            msg = client.recv(1024)
            msg = msg.split()

            if msg[0] == "GET_FILE" and len(msg) == 2:
                filename = msg[1]
                #print "Klient %s żąda pliku - ",filename % addr
                if(self.find_file(filename)):
                    if(self.send_file(client, filename) < 0):
                        client.close()
                else:
                    client.send("Plik nie istnieje!")




    def find_file(self,filename):
        path = os.listdir(self.path)
        if filename in path:
            return True
        else:
            return False


    def send_file(self,client,filename):
        p = self.path + filename
        print p
        try:
            fd = open(p,"r")
        except IOError:
            return -1

        data = fd.read()
        client.send(data)

        #odbiorca przesyla sygnal zakonczenia pobierania
        msg = client.recv(32)
        if msg == "DONE":
            #print "Klient %s sie rozlaczyl!" % addr
            client.close()

    def stop(self):
        self.alive = False

