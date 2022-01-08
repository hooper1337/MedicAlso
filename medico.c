#include "medico.h"

int balcao_fd, utente_fd, especialista_fd;

void trataSig(int i)
{
    (void)i;
    fprintf(stderr, "\nServidor do Especialista a terminar! Interrompido via teclado\n\n");
    close(especialista_fd);
    unlink(ESPECIALISTA_FIFO_FINAL);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    Pessoa utente;
    Balcao balcao;
    Pessoa especialista;
    especialista.pid = getpid();
    especialista.estado = 0;
    especialista.tipoPessoa = 2;
    int nfd;
    fd_set read_fds;
    struct timeval tv;

    strcpy(especialista.pNome, argv[1]);
    strcpy(especialista.uNome, argv[2]);
    strcpy(especialista.especialidade, argv[3]);

    if (signal(SIGINT, trataSig) == SIG_ERR)
    {
        perror("\nNão foi possivel configurar o sinal SIGINT\n");
        exit(EXIT_FAILURE);
    }

    sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, especialista.pid);
    if (mkfifo(ESPECIALISTA_FIFO_FINAL, 0777) == -1)
    {
        perror("\nmkfifo do especialista deu erro\n");
        exit(EXIT_FAILURE);
    }

    especialista_fd = open(ESPECIALISTA_FIFO_FINAL, O_RDWR | O_NONBLOCK);
    if (especialista_fd == -1)
    {
        perror("\nErro ao abrir fifo do especialista (RDWR/nonblocking\n");
        exit(EXIT_FAILURE);
    }

    balcao_fd = open(BALCAO_FIFO, O_RDWR | O_NONBLOCK);
    if (balcao_fd == -1)
    {
        fprintf(stderr, "\nO servidor não está a correr\n");
        unlink(UTENTE_FIFO_FINAL);
        exit(EXIT_FAILURE);
    }
    printf("\nMÉDICO [%d] CONFIGURADO!\n",getpid());
    printf("\n%s apresente-se ao serviço: \n", especialista.pNome);
    while (1)
    {

        tv.tv_sec = 50;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(especialista_fd, &read_fds);

        nfd = select(especialista_fd + 1, &read_fds, NULL, NULL, &tv);
        if (nfd == 0)
            printf("\nEstou a espera do utente!\n");
        if (nfd == -1)
        {
            printf("\nErro no select!\n");
        }

        if (FD_ISSET(0, &read_fds))
        {
            fgets(especialista.msg, sizeof(especialista.msg), stdin);

            // enviar para o balcao
            if (especialista.estado == 0)
            {
                write(balcao_fd, &especialista, sizeof(especialista));
            }
            else
            {
                write(utente_fd, &especialista, sizeof(especialista));
            }
        }

        if (FD_ISSET(especialista_fd, &read_fds))
        {
            // receber do utente
            if (especialista.estado == 0)
            {
                read(especialista_fd, &utente, sizeof(utente));
                printf("\nFoi-me atribuido um utente com o PID: %d\n", utente.pid);
                sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, utente.pid);
                utente_fd = open(UTENTE_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                if (utente_fd == -1)
                {
                    fprintf(stderr, "\nO Medico nao encontrou o utente!\n");
                }
                else
                {
                printf("\nConversa com o utente!\n");
                especialista.estado = 1;
                }
            }
            else
            {
                read(especialista_fd, &utente, sizeof(utente));
                printf("Mensagem do utente: %s \n", utente.msg);
            }
        }
    }
    return 0;
}