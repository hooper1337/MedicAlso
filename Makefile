default: all;

all: balcao.o utente.o medico.o

balcao.o: balcao.c
	gcc -pthread balcao.c -o balcao

utente.o: utente.c
	gcc utente.c -o utente

medico.o: medico.c
	gcc -pthread medico.c -o medico