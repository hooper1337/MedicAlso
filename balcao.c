#include "balcao.h"

int balcao_fd, utente_fd, especialista_fd;

void trataSig(int i)
{
    (void)i;
    fprintf(stderr, "\nServidor do Balcão a terminar! Interrompido via teclado\n\n");
    close(balcao_fd);
    unlink(BALCAO_FIFO);
    exit(EXIT_SUCCESS);
}

void adicionaNovaPessoa(Balcao aux, Pessoa pessoa, int total)
{
    if (pessoa.tipoPessoa == 1)
    {
        for (int i = 0; i < total; i++)
        {
            if (aux.utentes[i].pid == pessoa.pid)
            {
                break;
            }
            else
            {
                if (aux.utentes[i].pid == 0)
                {
                    aux.utentes[i] = pessoa;
                    break;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < total; i++)
        {
            if (aux.especialistas[i].pid == pessoa.pid)
            {
                break;
            }
            else
            {
                if (aux.especialistas[i].pid == 0)
                {
                    aux.especialistas[i] = pessoa;
                    break;
                }
            }
        }
    }
}

void *mostraListas(void *dados)
{
    Balcao* pdados = (Balcao*) dados;
    for(int i=0; i<sizeof(pdados->utentes); i++)
        printf("\nUtente [%d] - PID - [%d]",i, pdados->utentes[i].pid);

    printf("\n");
    for(int i=0; i<sizeof(pdados->especialistas); i++)
        printf("\nEspecialista [%d] - PID - [%d]", i, pdados->especialistas[i].pid);
    printf("\n");
}

void atribuiConsulta(Balcao* aux)
{
    for(int i = 0; i < sizeof(aux->especialistas); i++)
    {
        if(aux->especialistas[i].pid != 0 && aux->especialistas[i].estado == 0)
        {
            for(int j = 0; j < sizeof(aux->utentes); j++)
            {
                if(aux->utentes[j].pid != 0 && aux->utentes[j].estado == 0)
                {
                    sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, aux->utentes[j].pid);
                    sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, aux->especialistas[i].pid);
                    utente_fd = open(UTENTE_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                    especialista_fd = open(ESPECIALISTA_FIFO_FINAL, O_RDWR | O_NONBLOCK);

                    printf("\nConexão estabelecida!\nUtente- %d  Medico - %d\n", aux->utentes[j].pid, aux->especialistas[i].pid);
                    write(utente_fd, &aux->especialistas[i], sizeof(aux->especialistas[i]));
                    write(especialista_fd, &aux->utentes[j], sizeof(aux->utentes[j]));

                    aux->especialistas[i].estado = 1;
                    aux->utentes[j].estado = 1;

                    close(utente_fd);
                    close(especialista_fd);
                    break;

                }
            }
        }
    }
}

void main()
{
    // variáveis de ambiente
    if (getenv("MAXCLIENTES") == NULL)
    {
        printf("Erro! MAXCLIENTES nao esta definido! \n");
        exit(0);
    }
    else if (getenv("MAXMEDICOS") == NULL)
    {
        printf("Erro! MAXMEDICOS nao esta definido! \n");
        exit(0);
    }

    // definição de variáveis
    int canalEnvio[2], canalReceber[2];
    char str[40];
    Balcao balcao;
    balcao.utentes = malloc(atoi(getenv("MAXCLIENTES")) * sizeof(*balcao.utentes));
    balcao.especialistas = malloc(atoi(getenv("MAXMEDICOS")) * sizeof(*balcao.especialistas));
    Pessoa desconhecido;
    int nfd;
    fd_set read_fds;
    struct timeval tv;
    pthread_t mostraArrays;
    int aux=0;

    // cria pipe para envio de dados
    pipe(canalEnvio);
    // cria pipe para receber dados
    pipe(canalReceber);

    // inicializaArray(utentes, atoi(getenv("MAXCLIENTES")));
    if (signal(SIGINT, trataSig) == SIG_ERR)
    {
        perror("\nNão foi possivel configurar o sinal SIGINT\n");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(BALCAO_FIFO, 0777) == -1)
    {
        perror("\nmkfifo do FIFO do servidor deu erro\n");
        exit(EXIT_FAILURE);
    }

    balcao_fd = open(BALCAO_FIFO, O_RDWR | O_NONBLOCK);
    if (balcao_fd == -1)
    {
        perror("\nErro ao abrir fifo do balcão (RDWR/blocking\n");
        exit(EXIT_FAILURE);
    }
    printf("\nSERVIDOR DO BALCÃO CONFIGURADO!\n");

    while (1)
    {
        pthread_create(&mostraArrays, NULL, &mostraListas, &balcao);

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(balcao_fd, &read_fds);

        nfd = select(balcao_fd + 1, &read_fds, NULL, NULL, &tv);
        if (nfd == 0)
        {
            printf("\nEstou a espera de médicos e utentes!\n");
        }
        if (nfd == -1)
        {
            printf("\nErro no select!\n");
        }

        if (FD_ISSET(0, &read_fds))
        {
        }

        if (FD_ISSET(balcao_fd, &read_fds))
        {
            read(balcao_fd, &desconhecido, sizeof(desconhecido));
            if (desconhecido.tipoPessoa == 1)
            {
                sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, desconhecido.pid);
                adicionaNovaPessoa(balcao, desconhecido, atoi(getenv("MAXCLIENTES")));
                atribuiConsulta(&balcao);
            }
            else if (desconhecido.tipoPessoa == 2)
            {
                sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, desconhecido.pid);
                adicionaNovaPessoa(balcao, desconhecido, atoi(getenv("MAXMEDICOS")));
                atribuiConsulta(&balcao);
            }
        }
    }
}