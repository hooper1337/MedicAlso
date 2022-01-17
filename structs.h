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
#include <string.h>

#define TAM_MAX 256
#define BALCAO_FIFO "balcao_fifo"
#define SINAL_FIFO "sinal_fifo"
#define UTENTE_FIFO "utente_%d"
#define ESPECIALISTA_FIFO "especialista_%d"

char UTENTE_FIFO_FINAL[TAM_MAX];
char ESPECIALISTA_FIFO_FINAL[TAM_MAX];

// struct para utentes e medicos
struct pessoa
{
    char pNome[TAM_MAX];
    pid_t pid;
    int estado; // 0 para nao, 1 para sim
    char msg[TAM_MAX];
    char sintomas[TAM_MAX];
    char especialidade[TAM_MAX];
    int numConsulta;
    int prioridade;
    int tipoPessoa; // 1 para utente, 2 para medico
    int tempo;
} typedef Pessoa;


struct balcao
{
    int freq;
    int numUtentes;
    int numMedicos;
    Pessoa *utentes;
    Pessoa *especialistas;
} typedef Balcao;

void reset(Pessoa *aux);
void removerPessoa(Balcao *aux, int pid, int tipoPessoa, int maxMedicos, int maxUtentes);
void adicionaNovaPessoa(Balcao *aux, Pessoa pessoa, int maxMedicos, int maxUtentes);
void reset(Pessoa *aux);
void removerPessoa(Balcao *aux, int pid, int tipoPessoa, int maxMedicos, int maxUtentes);
void resetTempo(Balcao *aux, int pid, int maxMedicos);
void *aumentarTempo(void *dados);
void atribuiConsulta(Balcao *aux, int maxMedicos, int maxUtentes);
void comUtentes(Balcao *aux, int maxMedicos, int maxUtentes);
void comEspecialistas(Balcao *aux, int maxMedicos, int maxUtentes);
bool delUtX(Balcao* aux,char *nome, int maxMedicos,  int maxUtentes);
void *listaListas(void* dados);
void encerrar(Balcao *aux, int maxMedicos, int maxClientes);
int max(int a, int b);