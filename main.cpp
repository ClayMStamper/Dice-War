#include <iostream>
#include <unistd.h>
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"

static s die_1 = 0, die_2 = 0;

static pthread_mutex_t die_mutex;
static pthread_cond_t winner_cond;

int GetRand(int seed){
    int max = 6;
    srand(time(NULL) * seed);
    return rand() % max;
}

void* PlayerGo(void* t_id){

}

void* DealerGo(){

}

int main(int argc, char* argv[])
{

    if (argc != 2) {fprintf(stderr, "WARNING: %d arguments were passed! \n", argv[0]); exit(-1);}
    const int seed = atoi(argv[1]);

    pthread_t threads[5];
    pthread_attr_t  attr;

    pthread_mutex_init(&die_mutex, 0);
    pthread_cond_init(&winner_cond, 0);
    pthread_attr_init(&attr);

    return 0;
}
