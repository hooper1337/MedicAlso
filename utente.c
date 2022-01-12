#include "utente.h"

int balcao_fd, utente_fd, especialista_fd;

int main(int argc, char **argv)
{
    Pessoa utente;
    utente.pid = getpid();
    utente.estado = 0;
    utente.tipoPessoa = 1;
    Pessoa especialista;
    Pessoa desconhecido;
    int nfd;
    fd_set read_fds;
    struct timeval tv;
    bool primeiraVez = true;
    int recebi = 0;

    strcpy(utente.pNome, argv[1]);

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
            printf("\nEstou à escuta!\n");
        if (nfd == -1)
        {
            printf("\nNão tenho nada para receber/ler!\n");
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
                if(strcmp(utente.msg, "adeus\n")==0)
                {
                    write(balcao_fd, &utente, sizeof(utente));
                    write(especialista_fd, &utente, sizeof(utente));
                    close(especialista_fd);
                    close(balcao_fd);
                    close(utente_fd);
                    unlink(UTENTE_FIFO_FINAL);
                }
                else
                {
                    write(especialista_fd, &utente, sizeof(utente));
                }
                
            }
        }
        if (FD_ISSET(utente_fd, &read_fds))
        {
            if (utente.estado == 0)
            {
                if(recebi == 0)
                {
                    read(utente_fd, &utente, sizeof(utente));
                    printf("\nEspecialidade e pioridade: %s", utente.especialidade);
                    recebi = 1;
                }
                else
                {
                    read(utente_fd, &especialista, sizeof(especialista));
                    sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, especialista.pid);
                    especialista_fd = open(ESPECIALISTA_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                    if (especialista_fd == -1)
                    {
                        fprintf(stderr, "\nO Utente ainda não tem médico atribuido!\n");
                    }
                    else
                    {
                        printf("\nFoi-me atribuido um médico com o PID: %d\n", especialista.pid);
                        printf("\nConversa com o médico!\n\n");
                        utente.estado = 1;
                    }
                }
            }
            else
            {
                read(utente_fd, &especialista, sizeof(especialista));
                if(strcmp(especialista.msg, "adeus\n")==0)
                {
                    write(balcao_fd, &utente, sizeof(utente));
                    unlink(UTENTE_FIFO_FINAL);
                    close(balcao_fd);
                    close(utente_fd);
                    close(especialista_fd);
                    kill(getpid(), SIGTERM);  
                }
                printf("\nMensagem do médico: %s\n", especialista.msg);
            }
        }
    }
    return 0;
}