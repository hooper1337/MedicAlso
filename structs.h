#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TAM_MAX 256
#define BALCAO_FIFO "balcao_fifo"
#define UTENTE_FIFO "utente_%d"
#define ESPECIALISTA_FIFO "especialista_%d"

char UTENTE_FIFO_FINAL[TAM_MAX];
char ESPECIALISTA_FIFO_FINAL[TAM_MAX];

// struct para utentes e clientes
struct pessoa
{
    char nome[TAM_MAX];
    pid_t pid;
    int numcliente;
    int estado; // 0 para nao, 1 para sim
    char msg[TAM_MAX];
    char sintomas[TAM_MAX];
    char especialidade[TAM_MAX];
    int tipoPessoa; // 1 para utente, 2 para medico
} typedef Pessoa;


// struct para o balcao comunicar com os clientes
struct balcao
{
    Pessoa *utentes;
    Pessoa *especialistas;
} typedef Balcao;
