#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os
import socket
import threading
#import getopt
from clientfileserv import ClientFileServer

##Klasa Klient.
class Klient:

    ## Konstruktor klasy Klient. Ustawia katalog plików, jeżeli go nie ma, tworzy go.
    def __init__(self,dest=None):
        #self.address = (address,port)
        #Przed zalogowaniem klucz sesji ma inna wartosc niz 0
        if dest is None:
            try:
                os.mkdir('myfiles')
                self.myfilelist = os.listdir('myfiles')
            except OSError:
                self.myfilelist = os.listdir('myfiles')
        else:
            self.myfilelist = os.listdir(dest)

        self.session_key = -1
        self.is_connection = 0

    ## Tworzy gniazdo połączenia i nawiązuje połączenie z serwerem. Wykorzystuje adres i port serwera przekazany w funkcji live.
    def make_connection(self):
        if(self.is_connection == 1):
            print "Masz juz polaczenie z serwerem."
            return
        try:
            self.clientFd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.fileClient = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        except socket.error:
            self.clientFd = None
            self.fileClient = None
            print "Nie mozna utworzyc gniazda"
        try:
            self.clientFd.connect(self.address)
            #przywitaj sie
            self.is_connection = 1
            self.send_msg("HELLO")
            self.session_key = self.recv_message()
            if(self.session_key.find("ERROR") > 0):
                print "Połączenie nawiązane jednak liczba klientów została przekroczona. Proszę czekać...\n"
            else:
                print "Nawiazano połączenie pomyślnie. Twoj ID sesji to: %s\n" % self.session_key
        except socket.error, msg:
            print "Nie można nawiazać połaczenia. "
            return
    ## Przesyła do serwera listę plików z katalogu wskazanym w konstruktorze __init__ lub (jeżeli nie został wskazany katalog) myfiles.
    def send_file_list(self):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            return
        else:
            myfilelist = " ".join(self.myfilelist)
            dane_do_wyslania = "SHOW_FILES %s" % (myfilelist)
            self.send_msg(dane_do_wyslania)
            print "Wysłano listę plików!\n"


    def change_myfiles_dir(self, dirpath):
        self.myfilelist = os.listdir(dirpath)

    ## Wysyla do serwera pytanie o plik. Uzyskuje adres IP klienta, ktory plik posiada.
    def find_file(self, filename):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            return
        else:
            filename = filename + '\0'
            dane_do_wyslania = "FIND_FILE %s" % (filename)
            self.send_msg(dane_do_wyslania)

            odpowiedz = self.recv_message()
            print "Serwer: %s" % (odpowiedz)
            return
    ## Metoda pobiera plik od klienta. Max 4096 bajty
    def get_file(self,filename):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            return
        #przesyla o plik zapytanie do serwera
        filepath = filename + '\0'
        dane_do_wyslania = "GET_FILE %s" % (filepath)
        self.send_msg(dane_do_wyslania)
        cli_data = self.recv_message() #powinien otrzymac adres klienta
        #po otrzymaniu informacji o adresie IP klienta posiadajacego plik....
        address = (cli_data,9999)
        outfile = open("download/"+filename,"w",4096)
        #print address

        try:
            self.fileClient.connect(('127.0.0.1',9999))
            #self.fileClient.connect(address)
        except socket.error, (value,message):
            print "Nie mozna nawiac polaczenia z klientem!\n"
            return
        try:
            self.fileClient.send("GET_FILE "+filename)
            data = self.fileClient.recv(4096)
        except socket.error, (value,message):
            print "Nie mozna pobrac pliku.Prosze sprobowac ponownie pozniej.\n"
            return

        try:
            outfile.write(data)
        except IOError:
            print "Nie mozna zapisac pliku!"
            return

        try:
            self.fileClient.send("DONE")
            succes = 1
            print "Pobrano plik pomyślnie."
        except socket.error, (value,msg):
            print "Nie mozna zakonczyc polaczenia!"
            return

        if succes == 1:
            self.fileClient.close()
    ## Sprawdza, czy polaczenie TCP z serwerem jest nadal aktywne
    def check_connection(self):
        if self.is_connection > 0:
            return True
        else:
            return False

    ## Ogolna funkcja. Przesyla komunikat do serwera. Jezeli wystapi wyjatek, metoda zostanie przerwane i wyswietlony
    ## zostanie odpowiedni komunikat
    def send_msg(self, msg):
        if(self.check_connection() is False):
            print "Połączenie z serwerem zerwane!"
            return
        try:
            self.clientFd.send(msg)
        except NameError:
            print "Nie mozna wyslac danych!\n"
            return
        except socket.error, (value,message):
            print "Serwer odrzuca polaczenie! " + message
            return

    ## Ogólna funkcja. Odczytuje z serwera przesłany komunikat. Zapisuje do zmiennej 'odpowiedz' i zwraca go.
    ## Jezeli wystąpi wyjątek, metoda zostanie przerwana i wyświetlony zostanie komunikat.
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
    ## Pokazuje ID klienta przekazany przez serwer.
    def showMyID(self):
        print self.session_key

    ## Funkcja wykonywana w live(self) w pętli.
    def prompt(self):
        promp = raw_input("wup(live)# ")
        promp = promp.split()
        return promp

    ## Główna funkcja programu. Sprawdza polecenia przekazane do programu i na podstawie ich, wykonuje kolejne metody klasy Klient.
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
                    if len(promp) < 3:
                        continue
                    self.make_connection()
                    continue
                else:
                    print "Uzycie: connect <adres> <port>"
                    break
            elif promp[0] == 'send':
                self.send_file_list()
            elif promp[0] == 'get':
                if len(promp) == 2:
                    self.get_file(promp[1])

            elif promp[0] == 'find':
                if len(promp) == 2:
                    self.find_file(promp[1])
                else:
                    print "Uzycie: find <nazwa_szukanego_pliku>"
            elif promp[0] == 'id':
                self.showMyID()
            elif promp[0] == 'quit':
                self.pozegnaj_sie()
                sys.exit(1)
            elif promp[0] == 'bye':
                print "Zrywam połączenie z serwerem"
                self.pozegnaj_sie()
                self.is_connection = 0

            else:
                print "Nie rozpoznano polecenia. Wpisz help, aby uzyskac pomoc."
                continue
        return True



    ## Funkcja kończąca połączenie z serwerem.
    def pozegnaj_sie(self):
        self.send_msg("BYE")
## Czyści ekran.
def clean(self):
    print "\n" * 100

## Jak używać programu.
def usage():
    version()
    print "connect \t<adres> <port>\tNawiazuje polaczenie z serwerem"
    print "find \t\t<nazwa_pliku>\tPyta serwer, czy plik istnieje"
    print "send \t\t<nazwa_pliku>\tPrzesyła wskazany plik na serwer"
    print "get \t\t<nazwa_pliku>\tWysyła żadanie o plik"
    print '{0}\t\t\t\t{1}'.format('bye','Zrywa połączenie z serwerem')
    print '{0}\t\t\t\t{1}'.format('quit','Zamyka program')

    print "help\t\t\t\tWyswietla pomoc"

## Drukuje wersję programu, autora i inne informacje.
def version():
	print "Wersja programu: 1.0 Alfa, klient.\nAutor: Mariusz Skóra\nProjekt PAP - 2010/2011\n"


## Tworzenie instancji klas Klient() i ClientFileServer(). W razie przerwania aplikacji przez klienta (przechwycenie sygnału zakończenia ^C), wykonuje funkcję pozegnaj_sie().
def main():
    #uzyc getopt!
    threads = []
    w = ClientFileServer(9999)
    w.daemon = True
    w.start()

    kl = Klient()
    try:
        kl.live()
    except KeyboardInterrupt:
        #trzeba sprawdzic status polaczenia, czy nawiazano juz polaczenie z serwerem
        kl.pozegnaj_sie()
        print "\nZakończyłeś program."


if __name__=="__main__":
    main()

