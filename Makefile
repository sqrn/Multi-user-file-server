all:	serwer

serwer:	serwer.c
	gcc -Wall serwer.c include/* -o serwer
