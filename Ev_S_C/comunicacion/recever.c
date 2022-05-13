#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>

void *send(void *param)
{
    mqd_t mq1 = mq_open("/mq1", O_WRONLY);
    char str[64];
    char buffMenu[50];
    char *status = NULL;
    while (1)
    {
        fgets(buffMenu, sizeof(buffMenu), stdin);
        if (buffMenu[strlen(buffMenu) - 1] == '\n')
            buffMenu[strlen(buffMenu) - 1] = 0;
        if (strncmp(buffMenu, "exit", strlen("exit")) == 0)
        {
            break;
        }
        else if (strncmp(buffMenu, "./info_archivo.txt -open", strlen("./info_archivo.txt -open")) == 0)
        {
            FILE *inFile = fopen("./info_archivo.txt", "r");
            if (inFile == NULL)
            {
                perror("open file fails: ");
                exit(EXIT_FAILURE);
            }
            printf("¿Como quiere Eviarlo?\n");
            printf("De la primera linea a la ultima, envie: Orden\n");
            printf("De la ultima linea a la primera, envie: Reversa\n");
            //
            fgets(buffMenu, sizeof(buffMenu), stdin);
            if (buffMenu[strlen(buffMenu) - 1] == '\n')
                buffMenu[strlen(buffMenu) - 1] = 0;
            //
            if (strncmp(buffMenu, "Orden", strlen("Orden")) == 0)
            {
                do
                {
                    status = fgets(str, sizeof(str), inFile);
                    if (status != NULL)
                    {
                        mq_send(mq1, str, strlen(str) + 1, 0);
                        
                    }
                } while (status != NULL);
                
                fclose(inFile);
                memset(str, 0, 150);
                memset(buffMenu, 0, 10);
            }
            else if (strncmp(buffMenu, "Reversa", strlen("Reversa")) == 0)
            {
                int32_t lines = 16;
                int32_t nodo = 0;

                char words[lines][150];
                char buffer_info[150];

                while (fgets(buffer_info, 150, inFile))
                {
                    strtok(buffer_info, "\n");
                    memcpy(words[nodo], buffer_info, 150);
                    nodo++;
                }
                for (int i = lines; i >= 0; i--)
                {
                    mq_send(mq1, buffer_info, strlen(buffer_info) + 1, 0);
                    
                }

                
                fclose(inFile);
                memset(str, 0, 150);
                memset(buffMenu, 0, 10);
            }
            else if (strncmp(str, "exit", strlen("exit")) == 0)
            {
                exit(EXIT_SUCCESS);
            }
        }
    }
    mq_close(mq1);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

    pthread_t thread_1;
    pthread_create(&thread_1, NULL, &send, NULL);

    mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 150;
    attr.mq_curmsgs = 0;

    mq = mq_open("/mq0", O_RDONLY | O_CREAT, 0644, &attr);
    char buff[150];

    while (1)
    {
        mq_receive(mq, buff, 32, NULL);
        printf("Message received: %s\n", buff);
        if (strncmp(buff, "exit", strlen("exit")) == 0)
        {
            break;
        }
    }

    mq_close(mq);
    mq_unlink("/mq0");
    exit(EXIT_SUCCESS);
}