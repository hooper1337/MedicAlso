#include "balcao.h"

int balcao_fd, sinal_fd, utente_fd, especialista_fd, numConsulta = 0, listasDeEspera[5]={5,5,5,5,5};;


int max(int a, int b)
{
    return (a > b) ? a : b;
}

void adicionaNovaPessoa(Balcao *aux, Pessoa pessoa, int maxMedicos, int maxUtentes)
{
    if (pessoa.tipoPessoa == 1)
    {
        for (int i = 0; i < maxUtentes; i++)
        {
            if (aux->utentes[i].pid == pessoa.pid)
            {
                break;
            }
            else
            {
                if (aux->utentes[i].pid == 0)
                {
                    if(strcmp(pessoa.especialidade, "geral") == 0)
                    {
                        if(listasDeEspera[0] == 0)
                        {
                            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, pessoa.pid);
                            unlink(UTENTE_FIFO_FINAL);
                            kill(pessoa.pid, SIGTERM);
                            printf("\nO hospital não tem capacidade para mais utentes com a especialidade do mesmo!\n");
                            break;
                        }
                        else{
                            listasDeEspera[0]--;
                            aux->utentes[i] = pessoa;
                            aux->numUtentes++;
                            break;
                        }
                    }
                    else if(strcmp(pessoa.especialidade, "estomatologia") == 0)
                    {
                        if(listasDeEspera[1] == 0)
                        {
                            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, pessoa.pid);
                            unlink(UTENTE_FIFO_FINAL);
                            kill(pessoa.pid, SIGTERM);
                            printf("\nO hospital não tem capacidade para mais utentes com a especialidade do mesmo!\n");
                            break;
                        }
                        else{
                            listasDeEspera[1]--;
                            aux->utentes[i] = pessoa;
                            aux->numUtentes++;
                            break;
                        }
                    }
                    else if(strcmp(pessoa.especialidade, "ortopedia") == 0)
                    {
                        if(listasDeEspera[2] == 0)
                        {
                            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, pessoa.pid);
                            unlink(UTENTE_FIFO_FINAL);
                            kill(pessoa.pid, SIGTERM);
                            printf("\nO hospital não tem capacidade para mais utentes com a especialidade do mesmo!\n");
                            break;
                        }
                        else{
                            listasDeEspera[2]--;
                            aux->utentes[i] = pessoa;
                            aux->numUtentes++;
                            break;
                        }
                    }
                    else if(strcmp(pessoa.especialidade, "oftalmologia") == 0)
                    {
                        if(listasDeEspera[3] == 0)
                        {
                            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, pessoa.pid);
                            unlink(UTENTE_FIFO_FINAL);
                            kill(pessoa.pid, SIGTERM);
                            printf("\nO hospital não tem capacidade para mais utentes com a especialidade do mesmo!\n");
                            break;
                        }
                        else{
                            listasDeEspera[3]--;
                            aux->utentes[i] = pessoa;
                            aux->numUtentes++;
                            break;
                        }
                    }
                    else if(strcmp(pessoa.especialidade, "neurologia") == 0)
                    {
                        if(listasDeEspera[4] == 0)
                        {
                            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, pessoa.pid);
                            unlink(UTENTE_FIFO_FINAL);
                            kill(pessoa.pid, SIGTERM);
                            printf("\nO hospital não tem capacidade para mais utentes com a especialidade do mesmo!\n");
                            break;
                        }
                        else{
                            listasDeEspera[4]--;
                            aux->utentes[i] = pessoa;
                            aux->numUtentes++;
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < maxMedicos; i++)
        {
            if (aux->especialistas[i].pid == pessoa.pid)
            {
                aux->especialistas[i].estado = 0;
                break;
            }
            else
            {
                if (aux->especialistas[i].pid == 0)
                {
                    aux->especialistas[i] = pessoa;
                    aux->numMedicos++;
                    break;
                }
            }
        }
    }
}

void reset(Pessoa *aux)
{
    strcpy(aux->pNome, "\0");
    aux->pid = 0;
    aux->estado = 0; // 0 para nao, 1 para sim
    strcpy(aux->msg, "\0");
    strcpy(aux->sintomas, "\0");
    strcpy(aux->especialidade, "\0");
    aux->prioridade = 0;
    aux->tipoPessoa = 0; // 1 para utente, 2 para medico
    aux->tempo = 0;
}

void removerPessoa(Balcao *aux, int pid, int tipoPessoa, int maxMedicos, int maxUtentes)
{
    if (tipoPessoa == 1)
    {
        for (int i = 0; i < maxUtentes; i++)
        {
            if (aux->utentes[i].pid == pid)
            {
                reset(&aux->utentes[i]);
            }
        }
    }
    else if (tipoPessoa == 2)
    {
        for (int i = 0; i < maxMedicos; i++)
        {
            if (aux->especialistas[i].pid == pid)
            {
                reset(&aux->especialistas[i]);
            }
        }
    }
}

void resetTempo(Balcao *aux, int pid, int maxMedicos)
{
    for (int i = 0; i < maxMedicos; i++)
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
    int maxMedicos = atoi(getenv("MAXMEDICOS"));
    int maxUtentes = atoi(getenv("MAXMEDICOS"));
    while (1)
    {
        sleep(1);
        for (int i = 0; i < maxMedicos; i++)
        {
            if (pdados->especialistas[i].pid != 0)
            {
                pdados->especialistas[i].tempo++;
                if (pdados->especialistas[i].tempo >= 21)
                {
                    sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, pdados->especialistas[i].pid);
                    unlink(ESPECIALISTA_FIFO_FINAL);
                    removerPessoa(pdados, pdados->especialistas[i].pid, pdados->especialistas[i].tipoPessoa, maxMedicos, maxUtentes);
                    for (int x = 0; x < maxUtentes; x++)
                    {
                        if (pdados->especialistas[i].numConsulta == pdados->utentes[x].numConsulta)
                        {
                            removerPessoa(pdados, pdados->utentes[x].pid, pdados->utentes[x].tipoPessoa, maxMedicos, maxUtentes);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void atribuiConsulta(Balcao *aux, int maxMedicos, int maxUtentes)
{
    int recebeMaior;
    for (int i = 0; i < maxMedicos; i++)
    {
        if (aux->especialistas[i].pid != 0 && aux->especialistas[i].estado == 0)
        {
            for (int j = 0; j < maxUtentes; j++)
            {
                if (aux->utentes[j].pid != 0 && aux->utentes[j].estado == 0 && strcmp(aux->especialistas[i].especialidade, aux->utentes[j].especialidade) == 0)
                {
                    for (int i = 0; i < maxUtentes; i++)
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
                    for (int i = 0; i < maxUtentes; i++)
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
                    aux->utentes[j].numConsulta = numConsulta;

                    if(strcmp(aux->especialistas[i].especialidade, "geral")==0)
                    {
                        listasDeEspera[0]++;
                    }

                    else if(strcmp(aux->especialistas[i].especialidade, "estomatologia")==0)
                    {
                        listasDeEspera[1]++;
                    }

                    else if(strcmp(aux->especialistas[i].especialidade, "ortopedia")==0)
                    {
                        listasDeEspera[2]++;
                    }

                    else if(strcmp(aux->especialistas[i].especialidade, "oftalmologia")==0)
                    {
                        listasDeEspera[3]++;
                    }
                    else if(strcmp(aux->especialistas[i].especialidade, "neurologia")==0)
                    {
                        listasDeEspera[4]++;
                    }

                    close(utente_fd);
                    close(especialista_fd);
                    break;
                }
            }
        }
    }
}

void comUtentes(Balcao *aux, int maxMedicos, int maxUtentes)
{
    for (int i = 0; i < maxUtentes; i++)
    {
        if (aux->utentes[i].estado == 0)
        {
            if (aux->utentes[i].pid != 0)
                printf("\nNome: %s - %s %d - %d\n", aux->utentes[i].pNome, aux->utentes[i].especialidade, aux->utentes[i].prioridade, aux->utentes[i].pid);
        }
        else
        {
            for (int j = 0; j < maxMedicos; j++)
            {
                if (aux->utentes[i].numConsulta == aux->especialistas[j].numConsulta)
                {
                    printf("\nNome: %s - %s %d - %d a ser atendido por %s - %d\n", aux->utentes[i].pNome, aux->utentes[i].especialidade, aux->utentes[i].prioridade, aux->utentes[i].pid, aux->especialistas[j].pNome, aux->especialistas[j].pid);
                }
            }
        }
    }
}

void comEspecialistas(Balcao *aux, int maxMedicos, int maxUtentes)
{
    for (int i = 0; i < maxMedicos; i++)
    {
        if (aux->especialistas[i].estado == 0)
        {
            if (aux->especialistas[i].pid != 0)
                printf("\nNome: %s - %s - %d\n", aux->especialistas[i].pNome, aux->especialistas[i].especialidade, aux->especialistas[i].pid);
        }
        else
        {
            for (int j = 0; j < maxUtentes; j++)
            {
                if (aux->utentes[i].numConsulta == aux->especialistas[j].numConsulta)
                {
                    printf("\nNome: %s - %s - %d a atender %s - %d\n", aux->especialistas[i].pNome, aux->especialistas[i].especialidade, aux->especialistas[i].pid, aux->utentes[j].pNome, aux->utentes[j].pid);
                }
            }
        }
    }
}

bool delUtX(Balcao* aux,char *nome, int maxMedicos,  int maxUtentes)
{
    for(int i=0; i<maxUtentes; i++)
    {
        if(strcmp(aux->utentes[i].pNome, nome) == 0 && aux->utentes[i].estado == 0)
        {
            kill(aux->utentes[i].pid, SIGTERM);
            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, aux->utentes[i].pid);
            unlink(UTENTE_FIFO_FINAL);
            removerPessoa(aux, aux->utentes[i].pid, aux->utentes[i].tipoPessoa, maxMedicos, maxUtentes);
            printf("\nUtente %s eliminado!\n", aux->utentes[i].pNome);
            return true;
        }
    }
    return false;
}

bool delEspX(Balcao* aux,char *nome, int maxMedicos,  int maxUtentes)
{
    for(int i=0; i<maxMedicos; i++)
    {
        if(strcmp(aux->especialistas[i].pNome, nome) == 0 && aux->especialistas[i].estado == 0)
        {
            kill(aux->especialistas[i].pid, SIGTERM);
            sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, aux->especialistas[i].pid);
            unlink(ESPECIALISTA_FIFO_FINAL);
            removerPessoa(aux, aux->especialistas[i].pid, aux->especialistas[i].tipoPessoa, maxMedicos, maxUtentes);
            printf("\nEspecialista %s eliminado!\n", aux->especialistas[i].pNome);
            return true;
        }
    }
    return false;
}

void *listaListas(void* dados)
{
    Balcao *pdados = (Balcao *)dados;

    while (1)
    {
        sleep(pdados->freq);
        printf("\nLista de espera das especialidades\n\n");
        printf("\nGeral - %d", listasDeEspera[0]);
        printf("\nEstomatologia - %d", listasDeEspera[1]);
        printf("\nOrtopedia - %d", listasDeEspera[2]);
        printf("\nOftalmologia - %d", listasDeEspera[3]);
        printf("\nNeurologia - %d\n", listasDeEspera[4]);
    }
}

void encerrar(Balcao *aux, int maxMedicos, int maxClientes)
{
    for(int i=0; i < maxClientes; i++)
    {
        if(aux->utentes[i].pid != 0)
        {
            sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, aux->utentes[i].pid);
            unlink(UTENTE_FIFO_FINAL);
            kill(aux->utentes[i].pid, SIGTERM);
        }
    }

    for(int j=0; j < maxMedicos; j++)
    {
        if(aux->especialistas[j].pid != 0)
        { 
            sprintf(ESPECIALISTA_FIFO_FINAL, ESPECIALISTA_FIFO, aux->especialistas[j].pid);
            unlink(ESPECIALISTA_FIFO_FINAL);
            kill(aux->especialistas[j].pid, SIGTERM);
        }
    }
    unlink(BALCAO_FIFO);
    unlink(SINAL_FIFO);
    kill(getpid(), SIGTERM);
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
    int maxMedicos, maxUtentes;
    maxMedicos = atoi(getenv("MAXMEDICOS"));
    maxUtentes = atoi(getenv("MAXMEDICOS"));
    Balcao balcao;
    balcao.freq = 10;
    balcao.utentes = malloc(maxUtentes * sizeof(*balcao.utentes));
    balcao.especialistas = malloc(maxMedicos * sizeof(*balcao.especialistas));
    Pessoa desconhecido;
    char delim[] = " ";
    char comando[TAM_MAX];
    int nfd;
    fd_set read_fds;
    struct timeval tv;
    pthread_t mostralistasEspera;
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

    pthread_create(&aumentar, NULL, &aumentarTempo, &balcao);
    pthread_create(&mostralistasEspera, NULL, &listaListas, &balcao);

    close(canalEnvio[0]);
    close(canalReceber[1]);

    while (1)
    {
        tv.tv_sec = 25;
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
            fgets(comando, sizeof(comando), stdin);
            if (strcmp(comando, "utentes\n") == 0)
                comUtentes(&balcao, maxMedicos, maxUtentes);
            else if (strcmp(comando, "especialistas\n") == 0)
                comEspecialistas(&balcao, maxMedicos, maxUtentes);
            else if (comando[0] == 'd' && comando[1] == 'e' && comando[2] == 'l' && comando[3] == 'u' && comando[4] == 't')
            {
                char *ptr1 = strtok(comando, delim);
                ptr1 = strtok(NULL, delim);
                ptr1[strlen(ptr1)-1] = '\0';
                if(delUtX(&balcao, ptr1, maxMedicos, maxUtentes))
                {
                    printf("\nUtente %s removido com sucesso!\n", ptr1);
                }
                else
                    printf("\nO utente não existe ou está numa consulta!\n");
            }
            else if (comando[0] == 'd' && comando[1] == 'e' && comando[2] == 'l' && comando[3] == 'e' && comando[4] == 's' && comando[5] == 'p')
            {
                char *ptr1 = strtok(comando, delim);
                ptr1 = strtok(NULL, delim);
                ptr1[strlen(ptr1)-1] = '\0';
                if(delEspX(&balcao, ptr1, maxMedicos, maxUtentes))
                {
                    printf("\nMédico %s removido com sucesso!\n", ptr1);
                }
                else
                    printf("\nO médico não existe ou está numa consulta!\n");
            }
            else if(comando[0] == 'f' && comando[1] == 'r' && comando[2] == 'e' && comando[3] == 'q')
            {
                char *ptr2 = strtok(comando, delim);
                ptr2 = strtok(NULL, delim);
                balcao.freq = atoi(ptr2);
                printf("\nFrequência alterada para %d segundos!\n", balcao.freq);
            }
            else if(strcmp(comando, "encerrar\n")==0)
            {
                encerrar(&balcao, maxMedicos, maxUtentes);
            }
            else
            {
                printf("\nComando não reconhecido!\n");
            }
        }

        if (FD_ISSET(sinal_fd, &read_fds))
        {
            int pid;
            int size = read(sinal_fd, &pid, sizeof(pid));
            if (size == sizeof(pid))
            {
                resetTempo(&balcao, pid, maxMedicos);
            }
        }

        if (FD_ISSET(balcao_fd, &read_fds))
        {
            read(balcao_fd, &desconhecido, sizeof(desconhecido));
            if (desconhecido.tipoPessoa == 1)
            {
                if (balcao.numUtentes == maxUtentes)
                {
                    kill(desconhecido.pid, SIGTERM);
                    sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, desconhecido.pid);
                    unlink(UTENTE_FIFO_FINAL);
                    printf("\nNão posso trabalhar com mais utentes!\n");
                }
                else if (desconhecido.estado == 1)
                {
                    removerPessoa(&balcao, desconhecido.pid, desconhecido.tipoPessoa, maxMedicos, maxUtentes);
                }
                else
                {
                    if(strcmp(desconhecido.msg, "sair\n")==0)
                    {
                        printf("\nO utente %s - %d foi-se embora!\n", desconhecido.pNome, desconhecido.pid);
                        if(desconhecido.prioridade != 0)
                        {
                            removerPessoa(&balcao, desconhecido.pid, desconhecido.tipoPessoa, maxMedicos, maxUtentes);
                        }
                        sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, desconhecido.pid);
                        unlink(UTENTE_FIFO_FINAL);
                        kill(desconhecido.pid, SIGTERM);
                        if(strcmp(desconhecido.especialidade, "geral")==0 && desconhecido.estado == 0)
                        {
                            listasDeEspera[0]++;
                            close(utente_fd);
                        }

                        else if(strcmp(desconhecido.especialidade, "estomatologia")==0 && desconhecido.estado == 0)
                        {
                            listasDeEspera[1]++;
                            close(utente_fd);
                        }

                        else if(strcmp(desconhecido.especialidade, "ortopedia")==0 && desconhecido.estado == 0)
                        {
                            listasDeEspera[2]++;
                            close(utente_fd);
                        }

                        else if(strcmp(desconhecido.especialidade, "oftalmologia")==0 && desconhecido.estado == 0)
                        {
                            listasDeEspera[3]++;
                            close(utente_fd);
                        }
                        else if(strcmp(desconhecido.especialidade, "neurologia")==0 && desconhecido.estado == 0)
                        {
                            listasDeEspera[4]++;
                            close(utente_fd);
                        }
                    }
                    else{
                    write(canalEnvio[1], desconhecido.msg, strlen(desconhecido.msg));
                    read(canalReceber[0], desconhecido.especialidade, sizeof(desconhecido.msg) - 1);
                    sprintf(UTENTE_FIFO_FINAL, UTENTE_FIFO, desconhecido.pid);
                    utente_fd = open(UTENTE_FIFO_FINAL, O_RDWR | O_NONBLOCK);
                    write(utente_fd, &desconhecido.especialidade, sizeof(desconhecido.especialidade));
                    close(utente_fd);
                    // separar a mensagem do classificador
                    char *ptr = strtok(desconhecido.especialidade, delim);
                    // guardar a especialidade no utente
                    strcpy(desconhecido.especialidade, ptr);
                    ptr = strtok(NULL, delim);
                    // guardar a prioridade do utente
                    desconhecido.prioridade = atoi(ptr);
                    adicionaNovaPessoa(&balcao, desconhecido, maxMedicos, maxUtentes);
                    atribuiConsulta(&balcao, maxMedicos, maxUtentes);
                    }
                }
            }
            else if (desconhecido.tipoPessoa == 2)
            {
                if (balcao.numMedicos == maxMedicos)
                {
                    kill(desconhecido.pid, SIGTERM);
                    printf("\nNão posso trabalhar com mais médicos!\n");
                }
                adicionaNovaPessoa(&balcao, desconhecido, maxMedicos, maxUtentes);
                atribuiConsulta(&balcao, maxMedicos, maxUtentes);
            }
        }
    }
}