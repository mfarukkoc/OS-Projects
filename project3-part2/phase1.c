#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1

int main(void)
{
    int pid, pfd[2], i, n, status;
    char buf[40];
    if (pipe(pfd) < 0) // create a pipe
    {
        printf("Pipe1 creation error\n");
        return 1;
    }
    if ((pid = fork()) < 0)
    {
        printf("Fork error \n");
        return 2;
    }
    if (pid == 0)
    {
        /* ----- child executes here -----*/
        printf("<Child> mypid<%d>  ppid<%d>\n", getpid(), getppid());
        close(pfd[READ]);
        while (fgets(buf, 40, stdin) != 0)
        {
            if (buf[0] == '\n')
                continue;
            write(pfd[WRITE], buf, strlen(buf));
        }
        close(pfd[WRITE]);
        exit(0);
    }
    i = wait(&status);
    close(pfd[WRITE]);
    while ((n = read(pfd[READ], buf, 40)) > 0)
    {
        int i, count = 0, isNumeric = 1;
        for (i = 0; i < n; i++)
        {
            if (buf[i] == '\n')
            {
                if (isNumeric == 1)
                    count++;
                isNumeric = 1;
            }
            else if (buf[i] < '0' || buf[i] > '9')
                isNumeric = 0;
        }
        printf("%d Lines is only numeric\n", count);
    }
    return 0;
}
