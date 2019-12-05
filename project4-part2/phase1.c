#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
sem_t s_rd,s_sqrx,s_mul2;
int x;
void *sqr()
{
    int lx;
    sem_wait(&s_rd);
    lx = x;
    printf("computing x^2..\n");
    lx *= lx;
    sleep(7);
    sem_post(&s_sqrx);
    x=lx;
    return NULL;
}

void *mul2()
{
    int lx;
    sem_wait(&s_sqrx);
    lx = x;
    printf("computing 2*x^2..\n");
    lx *= 2;
    sleep(3);
    x=lx;
    sem_post(&s_mul2);

    return NULL;
}

int main()
{
    pthread_t mainTID, sqrTID, mulTID;
    mainTID = pthread_self();

    sem_init(&s_rd,0,0);
    sem_init(&s_sqrx,0,0);
    sem_init(&s_mul2,0,0);

    pthread_create(&sqrTID,NULL,sqr,NULL);
    pthread_create(&mulTID,NULL,mul2,NULL);
    
    printf("enter x:");
    scanf("%d", &x);
    printf("scanning input..\n");
    sleep(5);
    sem_post(&s_rd);
    sem_wait(&s_mul2);
    printf("2*x^2 = %d", x);
    pthread_join(sqrTID,NULL);
    pthread_join(mulTID,NULL);
    
}