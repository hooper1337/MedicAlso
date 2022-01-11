#include "balcao.h"

int balcao_fd, sinal_fd, utente_fd, especialista_fd, numConsulta = 0;

void trataSig(int i)
{
    (void)i;
    fprintf(stderr, "\nServidor do Balcão a terminar! Interrompido via teclado\n\n");
    close(balcao_fd);
    unlink(BALCAO_FIFO);
    close(sinal_fd);
    unlink(SINAL_FIFO);
    exit(EXIT_SUCCESS);
}

int max(int a, int b)
{
    return (a > b) ? a : b;
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

void reset(Pessoa *aux)
{
    strcpy(aux->pNome, "\0");
    strcpy(aux->uNome, "\0");
    aux->pid = 0;
    aux->estado = 0; // 0 para nao, 1 para sim
    strcpy(aux->msg, "\0");
    strcpy(aux->sintomas, "\0");
    strcpy(aux->especialidade, "\0");
    aux->prioridade = 0;
    aux->tipoPessoa = 0; // 1 para utente, 2 para medico
    aux->tempo = 0;
}

void removerPessoa(Balcao *aux, int pid, int tipoPessoa)
{
    if (tipoPessoa == 1)
    {
        for (int i = 0; i < sizeof(aux->utentes); i++)
        {
            if (aux->utentes[i].pid == pid)
            {
                reset(&aux->utentes[i]);
            }
        }
    }
    else if (tipoPessoa == 2)
    {
        for (int i = 0; i < sizeof(aux->especialistas); i++)
        {
            if (aux->especialistas[i].pid == pid)
            {
                reset(&aux->especialistas[i]);
            }
        }
    }
}

void *mostraListas(void *dados)
{
    Balcao *pdados = (Balcao *)dados;
    while (1)
    {
        sleep(5);
        for (int i = 0; i < sizeof(pdados->utentes); i++)
            printf("\nUtente [%d] - PID - [%d]", i, pdados->utentes[i].pid);

        printf("\n");
        for (int i = 0; i < sizeof(pdados->especialistas); i++)
            printf("\nEspecialista [%d] - PID - [%d] - Tempo - [%d]", i, pdados->especialistas[i].pid, pdados->especialistas[i].tempo);
        printf("\n");
    }
}

void resetTempo(Balcao *aux, int pid)
{
    for (int i = 0; i < sizeof(aux->especialistas); i++)
    {
        if (aux->especialistas[i].pid == pid)
        {
            aux->especialistas[i].tempo = 0;
        }
    }
}

void *aumentarTempo(void *dados)
{
    Balcao *pdados = (Balcao *)dados;
    while (1)
    {
        sleep(1);
        for (int i = 0; i < sizeof(pdados->especialistas); i++)
        {
            if (pdados->especialistas[i].pid != 0)
            {
                pdados->especialistas[i].tempo++;
                if (pdados->especialistas[i].tempo >= 21)
                {
                    sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, pdados->especialistas[i].pid);
                    unlink(ESPECIALISTA_FIFO_FINAL);
                    removerPessoa(pdados, pdados->especialistas[i].pid, pdados->especialistas[i].tipoPessoa);
                }
            }
        }
    }
}

void atribuiConsulta(Balcao *aux)
{
    int recebeMaior;
    for (int i = 0; i < sizeof(aux->especialistas); i++)
    {
        if (aux->especialistas[i].pid != 0 && aux->especialistas[i].estado == 0)
        {
            for (int j = 0; j < sizeof(aux->utentes); j++)
            {
                if (aux->utentes[j].pid != 0 && aux->utentes[j].estado == 0 && strcmp(aux->especialistas[i].especialidade, aux->utentes[j].especialidade) == 0)
                {
                    for (int i = 0; i < sizeof(aux->utentes); i++)
                    {
                        if (strcmp(aux->utentes[i].especialidade, aux->utentes[j].especialidade) == 0)
                        {
                            if (recebeMaior > aux->utentes[i].prioridade)
                            {
                                recebeMaior = aux->utentes[i].prioridade;
                            }
                            if (i == sizeof(aux->utentes))
                            {
                                break;
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                    for (int i = 0; i < sizeof(aux->utentes); i++)
                    {
                        if (strcmp(aux->utentes[i].especialidade, aux->utentes[j].especialidade) == 0)
                        {
                            if (aux->utentes[i].prioridade == recebeMaior)
                                break;
                            else
                                continue;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, aux->utentes[j].pid);
                    sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, aux->especialistas[i].pid);
                    utente_fd = open(UTENTE_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                    especialista_fd = open(ESPECIALISTA_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                    numConsulta++;
                    printf("\n\nConexão estabelecida!\nUtente- %d -> Medico - %d -> Consulta número - %d\n", aux->utentes[j].pid, aux->especialistas[i].pid, numConsulta);
                    write(utente_fd, &aux->especialistas[i], sizeof(aux->especialistas[i]));
                    write(especialista_fd, &aux->utentes[j], sizeof(aux->utentes[j]));

                    aux->especialistas[i].estado = 1;
                    aux->utentes[j].estado = 1;
                    aux->especialistas[i].numConsulta = numConsulta;
                    aux->especialistas[j].numConsulta = numConsulta;

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
    char delim[] = " ";
    int nfd;
    fd_set read_fds;
    struct timeval tv;
    pthread_t mostraArrays;
    pthread_t aumentar;

    // cria pipe para envio de dados
    pipe(canalEnvio);
    // cria pipe para receber dados
    pipe(canalReceber);

    if (fork() == 0)
    {
        close(0);
        // se estivermos associamos a extremidade de leitura do primeiro pipe ao stdin
        dup(canalEnvio[0]);
        close(canalEnvio[0]);
        // fechamos o que está a mais
        close(canalEnvio[1]);
        close(1);
        // agora associamos a extremidade de escrita do segundo pipe ao stdout
        dup(canalReceber[1]);
        // voltamos a fechar o que está mais
        close(canalReceber[1]);
        close(canalReceber[0]);
        // depois dos pipes configurados executamos o classificador
        execl("classificador", "classificador", NULL);
        exit(-1);
    }

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
        perror("\nErro ao abrir fifo do balcão\n");
        exit(EXIT_FAILURE);
    }
    printf("\nSERVIDOR DO BALCÃO CONFIGURADO!\n");

    if (mkfifo(SINAL_FIFO, 0777) == -1)
    {
        perror("\nmkfifo do FIFO dos sinais deu erro\n");
        exit(EXIT_FAILURE);
    }

    sinal_fd = open(SINAL_FIFO, O_RDWR | O_NONBLOCK);
    if (sinal_fd == -1)
    {
        perror("\nErro ao abrir fifo dos sinais\n");
        exit(EXIT_FAILURE);
    }
    printf("\nSINAIS DO BALCÃO CONFIGURADO!\n");

    pthread_create(&mostraArrays, NULL, &mostraListas, &balcao);
    pthread_create(&aumentar, NULL, &aumentarTempo, &balcao);

    close(canalEnvio[0]);
    close(canalReceber[1]);
    
    while (1)
    {
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(balcao_fd, &read_fds);
        FD_SET(sinal_fd, &read_fds);

        nfd = select(max(balcao_fd, sinal_fd) + 1, &read_fds, NULL, NULL, &tv);
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

        if (FD_ISSET(sinal_fd, &read_fds))
        {
            int pid;
            int size = read(sinal_fd, &pid, sizeof(pid));
            if (size == sizeof(pid))
            {
                printf("\nRECEBI! PID - %d\n", pid);
                resetTempo(&balcao, pid);
            }
        }

        if (FD_ISSET(balcao_fd, &read_fds))
        {
            read(balcao_fd, &desconhecido, sizeof(desconhecido));
            if (desconhecido.tipoPessoa == 1)
            {
                write(canalEnvio[1], desconhecido.msg, strlen(desconhecido.msg));
                read(canalReceber[0], desconhecido.especialidade, sizeof(desconhecido.msg) - 1);
                sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, desconhecido.pid);
                utente_fd = open(UTENTE_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                write(utente_fd, &desconhecido, sizeof(desconhecido));
                close(utente_fd);
                // separar a mensagem do classificador
                char *ptr = strtok(desconhecido.especialidade, delim);
                // guardar a especialidade no utente
                strcpy(desconhecido.especialidade, ptr);
                ptr = strtok(NULL, delim);
                // guardar a prioridade do utente
                desconhecido.prioridade = atoi(ptr);
                adicionaNovaPessoa(balcao, desconhecido, atoi(getenv("MAXCLIENTES")));
                atribuiConsulta(&balcao);
            }
            else if (desconhecido.tipoPessoa == 2)
            {
                adicionaNovaPessoa(balcao, desconhecido, atoi(getenv("MAXMEDICOS")));
                atribuiConsulta(&balcao);
            }
        }
    }
}