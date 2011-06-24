#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import socket
#import getopt

class Klient:
    port = 0
    address = ()
    session_key = -1
    clientFd = None
    username = ""
    is_connection = 0
    myfilelist = None;

    def __init__(self,dest=None):
        #self.address = (address,port)
        #Przed zalogowaniem klucz sesji ma inna wartosc niz 0
        if dest is None:
            self.myfilelist = os.listdir('/home/mariusz/workspace/cpp/projekt_pap/WUP/client/myfiles')
        else:
            self.myfilelist = os.listdir(dest)

        self.session_key = -1


    def make_connection(self):
        try:
            self.clientFd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error:
            self.clientFd = None
            print "Nie mozna utworzyc gniazda"
        try:
            self.clientFd.connect(self.address)
            #przywitaj sie
            self.send_msg("HELLO")
            self.session_key = self.recv_message()
            self.is_connection = 1
            if(self.session_key.find("ERROR") > 0):
                print "Połączenie nawiązane jednak liczba klientów została przekroczona. Proszę czekać...\n"
            else:
                print "Nawiazano połączenie pomyślnie. Twoj ID sesji to: %s\n" % self.session_key
        except socket.error, msg:
            print "Nie można nawiazać połaczenia. "
            return



    def send_file_list(self):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            sys.exit(1)
        else:
            myfilelist = " ".join(self.myfilelist)
            dane_do_wyslania = "SHOW_FILES %s" % (myfilelist)
            self.send_msg(dane_do_wyslania)
            print "Wysłano listę plików!\n"


    def change_myfiles_dir(self, dirpath):
        self.myfilelist = os.listdir(dirpath)
    """
    Wysyla do serwera pytanie o plik. Uzyskuje odpowiedz
    """
    def find_file(self, filename):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            sys.exit(1)
        else:
            dane_do_wyslania = "FIND_FILE %s" % (filename)
            self.send_msg(dane_do_wyslania)

            odpowiedz = self.recv_message()
            print "Serwer: %s" % (odpowiedz)
            return
    """
    Metoda pobiera plik z serwera.
    """
    def get_file(self,filename):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            sys.exit(1)
        else:
            dane_do_wyslania = "GET_FILE %s" % (filename)
            self.send_msg(dane_do_wyslania)
            odpowiedz = self.recv_message()

    """
    Metoda przesyla plik na serwer.
    """
    def send_file(self):
        pass

    """
    Sprawdza, czy polaczenie TCP z serwerem jest nadal aktywne
    """
    def check_connection(self):
        return True

    """
    Ogolna funkcja. Przesyla komunikat do serwera. Jezeli wystapi wyjatek, metoda zostanie przerwane i wyswietlony
    zostanie odpowiedni komunikat
    """
    def send_msg(self, msg):
        try:
            self.clientFd.send(msg)
        except NameError:
            print "Nie mozna wyslac danych!\n"
            return
        except socket.error, (value,message):
            print "Serwer odrzuca polaczenie! " + message
            return

    """
    Ogólna funkcja. Odczytuje z serwera przesłany komunikat. Zapisuje do zmiennej 'odpowiedz' i zwraca go.
    Jezeli wystąpi wyjątek, metoda zostanie przerwana i wyświetlony zostanie komunikat.
    """
    def recv_message(self):
        try:
            odpowiedz = self.clientFd.recv(80)
            return odpowiedz
        except socket.error, (value,message):
            print "Serwer odrzuca polaczenie: " + message
            return
        except NameError:
            print "Blad polaczenia\n"
            return


    def prompt(self):
        promp = raw_input("wup(live)# ")
        promp = promp.split()
        return promp

    def live(self):
        #clean()
        print "WUP - Wzajemne udostępnianie plików. Program klient-serwer. Wersja klient.\n\
        Proszę wpisać 'help', aby uzyskać listę poleceń"
        while True:
            promp = self.prompt()

            if promp[0] == 'help':
                usage()
            elif promp[0] == 'connect':
                if(len(promp) == 3):
                    self.address = (promp[1],int(promp[2]))
                    if(int(promp[2]) > 65535):
                        print "Port musi mieścić się w zakresie 0-65535\n"
                        continue
                    self.make_connection()
                    continue
                else:
                    print "Uzycie: connect <adres> <port>"
                    break
            elif promp[0] == 'send':
                self.send_file_list()
            elif promp[0] == 'find':
                self.find_file(promp[1])

            else:
                print "Nie rozpoznano polecenia. Wpisz help, aby uzyskac pomoc."
                continue
        return True




    def pozegnaj_sie(self):
        self.send_msg("BYE")

def clean(self):
    print "\n" * 100

def usage():
    version()
    print "connect <adres> <port>       Nawiazuje polaczenie z serwerem"
    print "find <nazwa_pliku>           Pyta serwer, czy plik istnieje"
    print "send <nazwa_pliku>           Przesyła wskazany plik na serwer"

    print "help                         Wyswietla pomoc"

def version():
	print "Wersja programu: 1.0 Alfa, klient.\nAutor: Mariusz Skóra\nProjekt PAP - 2010/2011\n"



def main():
    kl = Klient()
    try:
        kl.live()
    except KeyboardInterrupt:
        #trzeba sprawdzic status polaczenia, czy nawiazano juz polaczenie z serwerem
        kl.pozegnaj_sie()
        print "\nZakończyłeś program."


if __name__=="__main__":
    main()

