#include "common.h"

#define SSD "nand.txt"

char cash[5][20];
void sub_str(int* cnt, const char* org);
void cash_clear();

void write_ssd(int32_t idx, uint32_t val);
void read_ssd(int32_t idx);
void task(int32_t argc, void *argv[]);
void work();
int32_t check(FILE *fp, int32_t idx);

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

void sub_str(int* cnt, const char* org)
{
    uint32_t len = strlen(org);
    uint32_t pos = 0;

    for (int i = 0; i < len + 1; ++i)
    {
        if (org[i] == ' ' || org[i] == '\0')
        {
            cash[*cnt][pos] = '\0';
            ++(*cnt);
            pos = 0;
        }
        else
        {
            cash[*cnt][pos++] = org[i];
        }
    }
}

void cash_clear()
{
    for (int i = 0; i < 4; ++i)
    {
        memset(&cash[i], 0, 20);
    }
}

void write_ssd(int32_t idx, uint32_t val)
{
    FILE *fp = fopen(SSD, "r+w");
    if (check(fp, idx) == 0)
    {
        return;
    }

    uint8_t newStr[50];
    sprintf(newStr, "%X", val);
    uint32_t newStr_len = strlen(newStr);
    newStr[newStr_len] = '\n';
    newStr[newStr_len + 1] = '\0';

    uint8_t target[2000];
    memset(target, 0, 2000);
    uint8_t buffer[50];
    const char* crlf = "\n"; 
    int32_t curIdx = 0;
    uint32_t pos = 0;
    while (curIdx < 100)
    {
        ++curIdx;
        char* result = fgets(buffer, 50, fp);
        if (idx == curIdx)
        {
            memcpy(&target[pos], &newStr[0], strlen(newStr));
            pos += strlen(newStr);
        }
        else
        {
            if (result != NULL)
            {
                memcpy(&target[pos], &buffer[0], strlen(buffer));
                pos += strlen(buffer);
            }
            else
            {
                memcpy(&target[pos], crlf, strlen(crlf));
                pos += strlen(crlf);
            }
        }
    }
    fseek(fp, 0, SEEK_SET);
    fputs(target, fp);

    fclose(fp);
}

void read_ssd(int32_t idx)
{
    FILE *fp = fopen(SSD, "r+");
    FILE *result_fp = fopen(RESULT, "w+");
    if (check(fp, idx) == 0 || check(result_fp, idx) == 0)
    {
        return;
    }

    int32_t curIdx = 0;
    uint8_t buffer[50] = "\n\0";
    while (curIdx < idx)
    {
        if (fgets(buffer, 50, fp) == NULL)
            break;
        ++curIdx;
    }

    if (curIdx != idx)
        buffer[0] = '\0';
    else
        buffer[strlen(buffer) - 1] = '\0';

    fputs(buffer, result_fp);
    fclose(fp);
    fclose(result_fp);
    return ;
}

void task(int argc, void *argv[])
{
    if (argc == 4 && (((char*)argv[1])[0] == 'W' || ((char*)argv[1])[0] == 'w'))
    {
        int32_t idx = 0;
        uint32_t val = 0;
        sscanf(argv[2], "%d", &idx);
        sscanf(argv[3], "0x%X", &val);

        if (idx > 0 && val >= 0)
        {
            write_ssd(idx, val);
        }
        else
        {
            printf("%s <= not command", (char*)argv[3]);
        }
    }
    else if (argc == 3 && (((char*)argv[1])[0] == 'R' || ((char*)argv[1])[0] == 'r'))
    {
        int32_t idx = 0;
        sscanf(argv[2], "%d", &idx);

        if (idx > 0)
        {
            read_ssd(idx);
        }
        else
        {
            printf("%s <= not command", (char*)argv[2]);
        }
    }
}

void work()
{
    int fd;
    char buffer[50];

    unlink(PIPE_NAME);
    
    // 파이프 파일 권한 일단 풀로 준다 777
    if (mkfifo(PIPE_NAME, 0777) == -1) {
        printf("create pipe filed\n");
        return;
    }

    // 파이프 파일 읽기 모드로 연다.
    fd = open(PIPE_NAME, O_RDONLY);
    if (fd == -1) {
        printf("pipe open filed\n");
        return;
    }

    // 일단 루프 돌림
    while (1)
    {
        cash_clear();
        uint32_t bytes = read(fd, buffer, sizeof(buffer) - 1);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';
            int cnt = 0;
            sub_str(&cnt, buffer);

            // 포인터 배열로 캐스팅이 한번 필요하다..
            void* argv[4];
            for (int i = 0; i < cnt; ++i)
            {
                argv[i] = cash[i];
            }

            task(cnt, argv);
        }
        else if (bytes == 0)
        {
            printf("sender pipe close\n");
            // break 풀면 프로그램 종료
            break;
        }
        else
        {
            printf("error pipe\n");
            break;
        }
    }

    // 파이프 종료
    close(fd);
    unlink(PIPE_NAME);
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
