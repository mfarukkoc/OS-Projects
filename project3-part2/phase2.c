#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1

int main(void)
{
    int pid1, pfd1[2], pid2, pfd2[2], i, n, status;
    char buf[40];
    if (pipe(pfd1) < 0) // create a pipe
    {
        printf("Pipe1 creation error\n");
        return 1;
    }
    if (pipe(pfd2) < 0)
    {
        printf("Pipe2 creation error\n");
        return 2;
    }
    if ((pid1 = fork()) < 0)
    {
        printf("Fork error \n");
        return 2;
    }
    if (pid1 == 0)
    {
        //Child1 executes here
        printf("<Child1> mypid1<%d>  ppid1<%d>\n", getpid(), getppid());
        close(pfd1[READ]);
        close(pfd2[READ]);
        close(pfd2[WRITE]);
        while (fgets(buf, 40, stdin) != 0)
        {
            if (buf[0] == '\n')
                continue;
            write(pfd1[WRITE], buf, strlen(buf));
        }
        close(pfd1[WRITE]);
        exit(0);
    }
    else
    {
        i = wait(&status);
        if ((pid2 = fork()) < 0)
        {
            printf("Fork error");
        }
        if (pid2 == 0)
        {
            //Child2 executes here
            printf("<Child2> mypid1<%d>  ppid1<%d>\n", getpid(), getppid());
            close(pfd1[READ]);
            close(pfd1[WRITE]);
            close(pfd2[WRITE]);
            int count = 0;
            while ((n = read(pfd2[READ], buf, 40)) > 0)
            {
                int i;
                for (i = 0; i < n; i++)
                {
                    if (buf[i] == '\n')
                        count++;
                }
                printf("%d Lines contain only numeric.\n", count);
            }
            close(pfd2[READ]);
            exit(0);
        }
        close(pfd1[WRITE]);
        close(pfd2[READ]);
        while ((n = read(pfd1[READ], buf, 40)) > 0)
        {
            int i, isNumeric = 1, startPos = 0;
            for (i = 0; i < n; i++)
            {
                if (buf[i] == '\n')
                {
                    if (isNumeric == 1)
                    {
                        write(pfd2[WRITE], buf + startPos, i - startPos + 1);
                    }
                    startPos = i + 1;
                    isNumeric = 1;
                }
                else if (buf[i] < '0' || buf[i] > '9')
                    isNumeric = 0;
            }
        }
        close(pfd1[READ]);
        close(pfd2[WRITE]);
        return 0;
    }
}
