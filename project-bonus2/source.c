#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#define NBSLOTS  6
#define SLOTSIZE 500

 typedef struct { // shared queue structure definition
 char buf [NBSLOTS][SLOTSIZE]; // shared data buffer
 int tag [NBSLOTS]; // shared data tag array
 int in, out; // input and output slots indexes
 sem_t mutex_in; // mutex semaphore for producers
 sem_t free_slot; // counting semaphore free slots
 sem_t filled_slot; // counting semaphore filled slots
 } SharedQ_Def;

typedef struct { // input file descriptor
 int id; // file or producer id { 0 .. PRDMAX }
 char *fn; // input file path
 } File_Def;

SharedQ_Def sq; // define the shared circular queue ‘sq’
sem_t pCount;
int pActive=0;

void * producer(void * pFileDef)
{
    File_Def filedef = *((File_Def *) (pFileDef));
    printf("Producer[%d] started with path: %s\n\n", filedef.id, filedef.fn);
    int file;
    if((file = open(filedef.fn,O_RDONLY, 0)) == -1 )
    {
        fprintf(stderr, "\nOpening file path failed\n");
        return NULL;
    }
    char buf[SLOTSIZE];
    buf[SLOTSIZE] = '\0';
    int n;
    while((n=read(file,buf,SLOTSIZE)) > 0) {
        printf("\nProducer[%d] Reading \n",filedef.id);
        sem_wait(&(sq.free_slot));
        sem_wait(&(sq.mutex_in)); 
        sleep(1);
        memset(sq.buf[sq.in], 0, sizeof(sq.buf[sq.in]));
        memcpy(sq.buf[sq.in],buf,n);
        sq.tag[sq.in] = filedef.id;
        sq.in = (sq.in + 1) % NBSLOTS;
        sem_post(&(sq.filled_slot));
        sem_post(&(sq.mutex_in));
    }
    close(file);
     sem_wait(&pCount);
     pActive--;
     sem_post(&pCount);
     printf("\n-----------Producer[%d] ended ------------------------------\n",filedef.id);
    return NULL;
}

void * consumer(void * nProducer)
{
    printf("Consumer started with %d producer\n", (int)nProducer);
    int *recordCount = malloc((int)nProducer*sizeof(int));
    int i;
    int data;
    for( i=0; i < (int)nProducer; i++)
    {
       recordCount[i]=0; // initialize each record to 0
    }
    sem_getvalue(&sq.filled_slot,&data);
    while( pActive > 0 || data >0)
    {
        sem_wait(&sq.filled_slot);
        sleep(1);
        printf("Consumer Proccessing Producer[%d] -----------------\n%s\n",sq.tag[sq.out],sq.buf[sq.out]);
        i=0;
        for(i=0; i<SLOTSIZE; i++)
        {
            if(sq.buf[sq.out][i]=='\n') {
                recordCount[sq.tag[sq.out]]++;
            }
        }
        sq.out = (sq.out + 1 ) % NBSLOTS;
        sem_getvalue(&sq.filled_slot,&data);
        sem_post(&sq.free_slot);
    }
    for( i=0; i < (int)nProducer; i++)
    {
        printf("Producer %d has %d records.\n",i,recordCount[i]);
    }
    printf("\n-----------Consumer ended--------------------------------\n");
    return NULL;
}


void init_queue ( SharedQ_Def *pq ) {
    pq->in = 0; // init enqueuing index
    pq->out = 0; // init dequeing index
    sem_init( &(pq->mutex_in), 0, 1); // init mutex for enqueueing
    sem_init(&(pq->filled_slot),0,0);
    sem_init( &(pq->free_slot), 0, NBSLOTS); // nb of free slots
    return;
 }

 int main() {
     printf("Started \n");
     sem_init(&pCount,0,1);
     init_queue(&sq); // initialize shared queue
     pthread_t pTID[3];
     pthread_t cTID;
     File_Def f1 = { 0, "/etc/passwd"}; // 0 is the tag associated with this file
     File_Def f2 = { 1, "/etc/group" }; // 1 is the tag associated with this file
     // examples of input file descriptors
     pActive=2;
     pthread_create ( &cTID, NULL, consumer, (void *) pActive);
     pthread_create ( &pTID[0], NULL, producer, (void *) & f1);
     pthread_create ( &pTID[1], NULL, producer, (void *) & f2);
     pthread_join(pTID[0],NULL);
     pthread_join(pTID[1],NULL);
    //  pthread_join(pTID[2],NULL);
     pthread_join(cTID,NULL);
 }