#include "utente.h"

int balcao_fd, utente_fd, especialista_fd;

void trataSig(int i)
{
    (void)i;
    fprintf(stderr, "\nServidor do Utente a terminar! Interrompido via teclado\n\n");
    close(utente_fd);
    unlink(UTENTE_FIFO_FINAL);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    Pessoa utente;
    utente.pid = getpid();
    utente.estado = 0;
    utente.tipoPessoa = 1;
    Balcao balcao;
    Pessoa especialista;
    Pessoa desconhecido;
    int nfd;
    fd_set read_fds;
    struct timeval tv;
    bool primeiraVez = true;

    strcpy(utente.pNome, argv[1]);
    strcpy(utente.uNome, argv[2]);

    if (signal(SIGINT, trataSig) == SIG_ERR)
    {
        perror("\nNão foi possivel configurar o sinal SIGINT\n");
        exit(EXIT_FAILURE);
    }

    sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, utente.pid);
    if (mkfifo(UTENTE_FIFO_FINAL, 0777) == -1)
    {
        perror("\nmkfifo do utente deu erro\n");
        exit(EXIT_FAILURE);
    }

    utente_fd = open(UTENTE_FIFO_FINAL, O_RDWR | O_NONBLOCK);
    if (utente_fd == -1)
    {
        perror("\nErro ao abrir fifo do utente (RDWR/nonblocking\n");
        exit(EXIT_FAILURE);
    }

    balcao_fd = open(BALCAO_FIFO, O_RDWR | O_NONBLOCK);
    if (balcao_fd == -1)
    {
        fprintf(stderr, "\nO servidor não está a correr\n");
        unlink(UTENTE_FIFO_FINAL);
        exit(EXIT_FAILURE);
    }
    printf("\nUTENTE [%d] CONFIGURADO!\n",getpid());
    printf("\n%s escreva os seus sintomas: \n", utente.pNome);
    while (1)
    {
        tv.tv_sec = 50;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(utente_fd, &read_fds);

        nfd = select(utente_fd + 1, &read_fds, NULL, NULL, &tv);
        if (nfd == 0)
            printf("\nEstou a espera do médico!\n");
        if (nfd == -1)
        {
            printf("\nErro no select!\n");
        }
        if (FD_ISSET(0, &read_fds))
        {
            fgets(utente.msg, sizeof(utente.msg), stdin);
            if (utente.estado == 0)
            {
                write(balcao_fd, &utente, sizeof(utente));
            }
            else
            {
                write(especialista_fd, &utente, sizeof(utente));
            }
        }

        if (FD_ISSET(utente_fd, &read_fds))
        {
            if (utente.estado == 0)
            {
                read(utente_fd, &especialista, sizeof(especialista));
                printf("\nFoi-me atribuido um médico com o PID: %d\n", especialista.pid);
                sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, especialista.pid);
                especialista_fd = open(ESPECIALISTA_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                if (especialista_fd == -1)
                {
                    perror("\nMédico não encontrou: ");
                    printf("\n%d", especialista.pid);
                    fprintf(stderr, "\nO Utente nao encontrou o medico!\n");
                }
                else
                {
                printf("\nConversa com o médico!\n");
                utente.estado = 1;
                }
            }
            else
            {
                read(utente_fd, &especialista, sizeof(especialista));
                printf("\nMensagem do médico: %s\n", especialista.msg);
            }
        }
    }
    return 0;
}