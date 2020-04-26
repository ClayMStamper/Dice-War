#include <iostream>
#include <unistd.h>
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "string.h"
#include <random>

static int dieSums[] = {0, 0, 0, 0};
static const char playerNames[] = {'A', 'B', 'C', 'D'};

static pthread_mutex_t die_mutex;
static std::default_random_engine gen;
static std::uniform_int_distribution randDist;
static pthread_cond_t aTurn_cond, bTurn_cond, cTurn_cond, dTurn_cond, dealerTurn_cond;
static pthread_mutex_t aTurn_mutex, bTurn_mutex, cTurn_mutex, dTurn_mutex, dealerTurn_mutex;
static bool finished = false;


int GetRand(){
    int max = 6;
    return randDist(gen);
}

pthread_cond_t GetCondition(int id){
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
pthread_mutex_t GetMutex(int id){
    switch (id) {
        case 0:
            return aTurn_mutex;
            break;
        case 1:
            return bTurn_mutex;
            break;
        case 2:
            return cTurn_mutex;
            break;
        case 3:
            return dTurn_mutex;
            break;
        case 4:
            return dealerTurn_mutex;
            break;
    }
}

void* PlayerGo(void* id){

    //this threads id that maps one to one with it's name and conditions
    int myIndex = (long) id;

    //Get my mutex and wait condition
    pthread_cond_t myCond = GetCondition(myIndex);
    pthread_mutex_t myMutex = GetMutex(myIndex);

    while (!finished){
        //wait for dealer
        pthread_cond_wait(&myCond, &myMutex);

        //Get Partner index
        int partnerIndex = (myIndex + 2) % 4;

        //Roll dice
        pthread_mutex_lock(&die_mutex);

        if (finished)
            return nullptr;

        int rand1 = GetRand(), rand2 = GetRand();
        int myRoll = rand1 + rand2;
//        std::cout << "\n***updating " << playerNames[myIndex] << ": from " << dieSums[myIndex] << " to " << myRoll
//        << " (compare to partner = " << dieSums[partnerIndex] << ")";

        dieSums[myIndex] = myRoll;

        std::cout << "\nPLAYER " << playerNames[myIndex] << " gets " << rand1 << " and " << rand2 << " for a sum of " << dieSums[myIndex];
        pthread_mutex_unlock(&die_mutex);

        if (myRoll == dieSums[partnerIndex]){
            finished = true;
        }
        sleep(0.1f);
        //signal dealer that we're gucci
        pthread_cond_signal(&dealerTurn_cond);

    }


}

void* DealerGo(void* id){

    //used to keep track of who won
    int winner = 0;

    srand(time(nullptr));

    int startingIndex = (rand() % 4) - 1;
//    std::cout << "\nStarting with player: " << playerNames[startingIndex] << "\n\n";

    while (!finished) {
        //loop through players one by one and signal to roll dice
        for (int i = startingIndex; i < 4; ++i) {
            if (finished) {
                winner = i;
            } else {
                //signal player
                pthread_cond_t playerCond = GetCondition(i);
                pthread_cond_signal(&playerCond);
                //wait for response
                pthread_cond_wait(&dealerTurn_cond, &dealerTurn_mutex);
            }
        }
    }

    int winnerPartner = (winner + 2) % 4;

    sleep(1);
    std::cout << "\nDEALER: The winning team is " << playerNames[winner] << " and " << playerNames[winnerPartner] << "\n";

}

int main(int argc, char* argv[])
{

    if (argc != 2) {fprintf(stderr, "WARNING: %s arguments were passed! \n", argv[0]); exit(-1);}
    int seed = atoi(argv[1]);

    //init random vars for dice rolling
    gen.seed(seed);
    randDist = std::uniform_int_distribution(1, 6);

    //int random seed for start index

    pthread_t threads[5];
    pthread_attr_t  attr;

    pthread_mutex_init(&die_mutex, nullptr);
    pthread_attr_init(&attr);

    for (int i = 0; i < 5; ++i) {

        //init player turn conditions
        pthread_cond_t condition = GetCondition(i);
        pthread_cond_init(&condition, nullptr);

        //init player turn mutexes
        pthread_mutex_t mutex = GetMutex(i);
        pthread_mutex_init(&mutex, nullptr);

    }

    //create dealer thread
    pthread_create(&threads[4], &attr, DealerGo, (void*) 4);

    for (int i = 0; i < 4; ++i) {
        pthread_create(&threads[i], &attr, PlayerGo, (void*) i);
    }

    while (!finished){} //busy wait

    for (auto & thread : threads) {
        pthread_join(thread, nullptr);
    }

//    std::cout << "\nfinal id is: " << currentPlayer << "en";

    return 0;
}
