/*
1700001623 - Mehmet Faruk Koc
1401020023 - Ugurcan Topcu
*/

#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSZ 5000
#define imax 100000
#define jmax 100000


void *cpubound()
{
    int i, j;
    double x, y;
    printf("\n%d> (%u) computing\n", getpid(), pthread_self());
    for (i = 0; i < imax; i++)
        for (j = 0; j < jmax; j++)
            x = (x + 5) * (y + 1) * (y + 2);
    return NULL;
}

void *iobound(void *path)
{
    //...
    printf("iobound %d> thread(%u) started  Copying:%s\n", getpid(),pthread_self(), path);
    char buf[BUFSZ];
    int fdpath, fdout, n;
    if ((fdpath = open((char *)path, O_RDONLY, 0)) == -1)
    {
        fprintf(stderr, "\nOpening path failed\n");
        return NULL;
    }
    if ((fdout = open(basename((char *)path), O_FSYNC | O_WRONLY | O_CREAT | O_TRUNC ,
    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1 )
    {
        fprintf(stderr, "\nCreating file failed\n");
        return NULL;
    }
    while ((n = read(fdpath, buf, BUFSZ)) > 0)
    {
        write(fdout, buf, n-1);
    }
    return NULL;
}

int main()
{
    time_t start, tend;
    pthread_t s1TID, s2TID, s3TID, s4TID, s5TID;
    printf("\n----testing multi thread performance ----\n");
    if ((start = time(NULL)) == -1)
    {
        perror("time -1");
        return 1;
    }
    pthread_create(&s1TID, NULL, iobound, (void *)"/usr/bin/Xvnc");
    pthread_create(&s2TID, NULL, iobound, (void *)"/usr/bin/Xorg");
    pthread_create(&s3TID, NULL, cpubound, NULL);
    pthread_create(&s4TID, NULL, cpubound, NULL);
    pthread_create(&s5TID, NULL, cpubound, NULL);
    pthread_join(s1TID, NULL);
    pthread_join(s2TID, NULL);
    pthread_join(s3TID, NULL);
    pthread_join(s4TID, NULL);
    pthread_join(s5TID,NULL);
    if ((tend = time(NULL)) == -1)
    {
        perror("time -1");
        return 1;
    }
    printf("\nelapsed= %d\n", tend - start);
    //-------------------------------------------------------------------------
    printf("\n----testing single threaded performance ----\n");
    if ((start = time(NULL)) == -1)
    {
        perror("time -1");
        return 1;
    }
    iobound("/usr/bin/Xvnc");
    iobound("/usr/bin/Xorg");
    cpubound();
    cpubound();
    cpubound();
    if ((tend = time(NULL)) == -1)
    {
        perror("time -1");
        return 1;
    }
    printf("\nelapsed= %d\n", tend - start);
}