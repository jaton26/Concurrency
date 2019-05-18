#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h> 
#include <time.h>

volatile bool stop = false;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER; 

typedef struct{
    char bring;
    int fedCount;
    bool gotH, gotF, gotS;
} Customer;

struct{
    bool h, f, s;
} food;


void* chef(void *arg){
    int counter = 0;
    while (!stop){
        pthread_mutex_lock(&m);
        counter++;

        if (counter == 100){
            stop = true;
            pthread_mutex_unlock(&m);
            break;
        }

        while (food.h != false || food.f != false || food.s != false)
            pthread_cond_wait(&c, &m);

        pthread_mutex_unlock(&m);
    }
}

void setItem(int item){
    switch(item){
        case 0: food.h = true; break;
        case 1: food.f = true; break;
        case 2: food.s = true; break;
        default: break;
    }
}

void makefood(){
    int item1, item2;
    srand(time(0));
    item1 = rand()% 3; 
    do{
        srand(time(0));
        item2 = rand() % 3;
    } while(item1 == item2); 
    
    setItem(item1);
    setItem(item2);
}


void* eat(void *args){
    Customer *cust = (Customer *) args;
    while (!stop){
        printf("%c", cust->bring);
    }
}

int main(){
    Customer cust0 = (Customer){.bring = 'h', .fedCount = 0, .gotH = true, .gotF = false, .gotS = false};
    Customer cust1 = (Customer){.bring = 'f', .fedCount = 0, .gotH = false, .gotF = true, .gotS = false};
    Customer cust2 = (Customer){.bring = 's', .fedCount = 0, .gotH = false, .gotF = false, .gotS = true};

    food.h = true;
    food.f = true;
    food.s = true;

    pthread_t eat0, eat1, eat2, cook;

    pthread_create(&eat0, NULL, eat,  &cust0);
    pthread_create(&eat1, NULL, eat,  &cust1);
    pthread_create(&eat2, NULL, eat,  &cust2);
    pthread_create(&cook, NULL, chef, NULL);

   
    pthread_mutex_lock(&m);
    food.h = false;
    food.f = false;
    food.s = false;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);

    pthread_join(eat0, NULL);
    pthread_join(eat1, NULL);
    pthread_join(eat2, NULL);

    return 0;
}