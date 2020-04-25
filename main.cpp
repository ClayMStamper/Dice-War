#include <iostream>
#include <unistd.h>
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "string.h"
#include <random>

static int dieSums[] = {0, 0, 0, 0};
static char playerNames[] = {'A', 'B', 'C', 'D'};

static pthread_mutex_t die_mutex;
static std::default_random_engine gen;
static std::uniform_int_distribution randDist;
static pthread_cond_t winner_cond, aTurn_cond, bTurn_cond, cTurn_cond, dTurn_cond, dealerTurn_cond;

int GetRand(){
    int max = 6;
    return randDist(gen);
}

pthread_cond_t GetMyCondition(int id){
    switch (id) {
        case 0:
            return aTurn_cond;
            break;
        case 1:
            return bTurn_cond;
            break;
        case 2:
            return cTurn_cond;
            break;
        case 3:
            return dTurn_cond;
            break;
        case 4:
            return dealerTurn_cond;
            break;
    }
}

void* PlayerGo(void* id){

    int i = (long) id;

    //wait for signal from dealer
    pthread_cond_t myCond = GetMyCondition(i);
    pthread_cond_signal(&myCond);

    //Roll dice
    pthread_mutex_lock(&die_mutex);
    int rand1 = GetRand(), rand2 = GetRand();
    dieSums[i] = rand1 + rand2;
    std::cout << "\nPLAYER " << playerNames[i] << " gets " << rand1 << " and " << rand2 << " for a sum of " << dieSums[i];
    pthread_mutex_unlock(&die_mutex);

    //signal dealer that we're gucci
    pthread_cond_signal(&dealerTurn_cond);

}

void* DealerGo(){

}

int main(int argc, char* argv[])
{

    if (argc != 2) {fprintf(stderr, "WARNING: %d arguments were passed! \n", argv[0]); exit(-1);}
    int seed = atoi(argv[1]);

    gen.seed(seed);
    randDist = std::uniform_int_distribution(1, 6);

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
