#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1
int main()
{
    int pid1, pid2, pfd[2], fd, ret;
    if (pipe(pfd) < 0)
    { // create a pipe
        printf("Pipe creation error\n");
        return 1;
    }
    // create child1 process
    if ((pid1 = fork()) < 0)
    {
        printf("Fork error \n");
        return 2;
    }
    if (pid1 == 0)
    { /* ----- child1 executes here -----*/
        printf("<Child1> mypid<%d>  ppid<%d>\n", getpid(), getppid());
        close(pfd[READ]);    // close read end of the pipe
        dup2(pfd[WRITE], 1); // overwrite stdout with pipe
        close(pfd[WRITE]);   // close write end of the pipe
        ret = execlp("cat", "cat" ,"/etc/passwd",NULL);
        if (ret == -1)
        {
            perror("execlp");
            exit(1);
        }
        exit(0);
    } /* ------- end of child code -----------*/
    else
    {
        wait(); // wait child1 to terminate
        if ((pid2 = fork()) < 0)
        {
            printf("Fork error \n");
            return 3;
        }
        if (pid2 == 0)
        {
            /* ----- child2 executes here -----*/
            printf("<Child2> mypid<%d>  ppid<%d>\n", getpid(), getppid());
            close(pfd[WRITE]);  // close write end of the pipe
            dup2(pfd[READ], 0); // // overwrite stdin with pipe
            close(pfd[READ]); // close read end of the pipe
            ret = execlp("grep", "grep", "/var",NULL);
            if (ret == -1)
            {
                perror("execlp");
                exit(1);
            }
            exit(0);
        }
        else
        {
            /*--- Parent continues from here ------*/
            close(pfd[READ]);  // close read end of the pipe
            close(pfd[WRITE]); // close write end of the pipe
            wait(); // wait child2 to terminate
            return 0;
        }
    }
}