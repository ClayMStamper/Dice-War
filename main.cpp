#include <iostream>
#include <unistd.h>
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "string.h"


static int die_sums[] = {0, 0, 0, 0};

static pthread_mutex_t die_mutex;
static pthread_cond_t winner_cond;
static int seed = 0;

int GetRand(){
    int max = 6;
    srand(time(NULL) * seed);
    return rand() % max;
}

char GetName(int id){
    switch (id) {
        case 0:
            return 'A';
            break;
        case 1:
            return 'B';
            break;
        case 2:
            return 'C';
            break;
        case 3:
            return 'D';
            break;
    }
}

void* PlayerGo(void* id){

    int myId = (long) id;


    pthread_mutex_lock(&die_mutex);
//    printf("Player %s had sum: %d", GetName(myId), die_sums[myId]);
    sleep(1.0f);
    die_sums[myId] = GetRand();
    std::cout << "\nPlayer " << GetName(myId) << " has a last sum of " << die_sums[myId];
    pthread_mutex_unlock(&die_mutex);

}

void* DealerGo(){

}

int main(int argc, char* argv[])
{

    if (argc != 2) {fprintf(stderr, "WARNING: %d arguments were passed! \n", argv[0]); exit(-1);}
    seed = atoi(argv[1]);

    pthread_t threads[5];
    pthread_attr_t  attr;

    pthread_mutex_init(&die_mutex, 0);
    pthread_cond_init(&winner_cond, 0);
    pthread_attr_init(&attr);

    for (int i = 0; i < 4; ++i) {
        pthread_create(&threads[0], &attr, PlayerGo, (void*) i);
    }

    for (int i = 0; i < 4; ++i) {
        pthread_join(threads[i], 0);
    }

//    std::cout << "\nfinal id is: " << currentPlayer << "en";

    return 0;
}
