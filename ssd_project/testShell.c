#include "common.h"

void send_write(int32_t idx, uint32_t value);
void send_read(int32_t idx);
void send_full_write(uint32_t value);
void send_full_read();
void help();
int32_t check(FILE *fp, int32_t idx);

int fd;

int main(int argc, void *argv[])
{
    char buffer[50];

    fd = open(PIPE_NAME, O_WRONLY);
    if (fd == -1)
    {
        printf("pipe open failed\n");
        return 0;
    }

    while (1)
    {
        fgets(buffer, 50, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        if (strncmp("write", buffer, strlen("write")) == 0)
        {
            int32_t idx = -1;
            uint32_t value = 0;
            sscanf(buffer + strlen("write") + 1, "%d 0x%X", &idx, &value);
            if (idx > 0)
                send_write(idx, value);
        }
        else if (strncmp("read", buffer, strlen("read")) == 0)
        {
            int32_t idx = -1;
            sscanf(buffer + strlen("read") + 1, "%u", &idx);
            if (idx > 0)
                send_read(idx);
        }
        else if (strncmp("fullwrite", buffer, strlen("fullwrite")) == 0)
        {
            uint32_t value = 0;
            sscanf(buffer + strlen("fullwrite") + 1, "0x%X", &value);
            send_full_write(value);
        }
        else if (strncmp("fullread", buffer, strlen(buffer)) == 0)
        {
            send_full_read();
        }
        else if (strncmp("help", buffer, strlen(buffer)) == 0)
        {
            help();
        }
        else if (strncmp("exit", buffer, strlen(buffer)) == 0)
        {
            break;
        }
    }

    close(fd);
    return 0;
}

void send_write(int32_t idx, uint32_t value)
{
    usleep(30000);
    char buffer[50];
    sprintf(buffer, "ssd W %d 0x%X", idx, value);

    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        printf("pipe write falied\n");
    }
}
void send_read(int32_t idx)
{
    usleep(30000);
    char buffer[50];
    sprintf(buffer, "ssd R %d", idx);

    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        printf("pipe read falied\n");
    }

    usleep(20000);
    FILE *fp = fopen(RESULT, "r+");
    if (check(fp, idx) == 0)
    {
        return;
    }

    // 버퍼는 재사용한다.
    fgets(buffer, 50, fp);
    printf("%s\n", buffer);
    fclose(fp);
}
void send_full_write(uint32_t value)
{
    for (int32_t i = 1; i <= 100; ++i)
    {
        send_write(i, value);
    }
}
void send_full_read()
{
    for (int32_t i = 1; i <= 100; ++i)
    {
        send_read(i);
    }
}
void help()
{
    printf("write : write number 0x4byte(hex)\n");
    printf("read : read number\n");
    printf("fullwrite : fullwrite 0x4byte(hex)\n");
    printf("fullread : fullread\n");
    printf("exit : end\n");
}

int32_t check(FILE *fp, int32_t idx)
{
    if (fp == NULL)
    {
        printf("open error\n");
        return 0;
    }

    if (idx > 0 && idx > 100)
    {
        printf("overflow\n");
        return 0;
    }
    return 1;
}
