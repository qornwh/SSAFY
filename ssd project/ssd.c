#include "common.h"

#define SSD "nand.txt"

void write_ssd(uint32_t idx, uint32_t val);
int read_ssd(int idx);
void task(int argc, void *argv[]);
void work();
uint8_t write_check(const uint8_t *str);
uint8_t read_check(const uint8_t *str);

int main(int argc, void *argv[])
{
    if (argc > 1)
    {
        // 이때는 단일로 실행
        task(argc, argv);
    }
    else
    {
        // 이때는 독립적인 프로세스
        work();
    }
    return 0;
}

void write_ssd(uint32_t idx, uint32_t val)
{
    FILE *fp = fopen(SSD, "r+w");
    if (fp == NULL)
    {
        printf("open error\n");
        return;
    }

    uint8_t str[50];
    sprintf(str, "%X", val);
    str[strlen(str)] = '\n';

    uint8_t cash[50];
    uint32_t curIdx = 0;
    while (1)
    {
        if (idx - 1 == curIdx)
        {
            fputs(str, fp);
            break;
        }
        ++curIdx;
        if (fgets(cash, 50, fp) == NULL)
            break;
    }

    fclose(fp);
}

int read_ssd(int idx)
{
    FILE *fp = fopen(SSD, "+rw");

    fclose(fp);
    return 0;
}

void task(int argc, void *argv[])
{
    if (argc == 5 && *(char*)(argv[2] + 0) == 'W')
    {
        uint32_t idx = -1;
        uint32_t val = -1;
        sscanf(argv[3], "%d", &idx);
        sscanf(argv[4], "0x%X", &val);

        if (idx > 0 && val >= 0)
        {
            write_ssd(idx, val);
        }
        else
        {
            printf("%s <= not command", (char*)argv[4]);
        }
    }
    else if (argc == 4 && *(char*)(argv[2] + 0) == 'R')
    {
        
    }
}

void work()
{
    // 파이프로 읽어낸다.
    // 리눅스 종료시 같이 종료한다
    while (1)
    {
    }
}

uint8_t write_check(const uint8_t *str)
{
    if (12 > strlen(str) || strlen(str) > 13)
        return 0;

    return 1;
}

uint8_t read_check(const uint8_t *str)
{
    return 0;
}
