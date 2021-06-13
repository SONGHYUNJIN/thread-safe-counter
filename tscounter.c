#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

unsigned int loop_cnt;
counter_t counter;

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

#define PATH "/home/jongchank/key"

int sharedVal = 0;
int semid;

void s_wait() {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = 0;

    if (semop(semid, &buf, 1) == -1) {
        printf("semop() fail\n");
        exit(0);
    }
}

void s_quit() {
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = 0;

    if (semop(semid, &buf, 1) == -1) {
        printf("semop() fail\n");
        exit(0);
    }
}


void* mythread(void* arg)
{
    char* letter = arg;
    int i;

    printf("%s: begin\n", letter);
    for (i = 0; i < loop_cnt; i++) {
        s_wait();
        sharedVal++:
        s_quit();
    }
    printf("%s: done\n", letter);
    return NULL;
}

int main(int argc, char* argv[])
{
    key_t key;
    union semun arg;

    key = ftok(PATH, 'z');
    if (key < 0) {
        perror(argv[0]);
        exit(1);
    }
    semid = semget(key, 1, 0600 | IPC_CREAT);
    if (semid < 0) {
        perror(argv[0]);
        exit(1);
    }

    loop_cnt = atoi(argv[1]);

    init(&counter);

    pthread_t p1, p2;
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("semid = %d\n", semid);

    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

  

    
    printf("getting sem\n");
    for (int i = 0; i < 10; i++) {
        printf("%s in critical section\n", argv[0]);
        sleep(1);
    }
    printf("releasing sem\n");
   
    return 0;
}
