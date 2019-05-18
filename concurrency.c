#include <stdio.h>
#include <stdbool.h>
#include <pthread.h> 

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
} Food;


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
        while (Food.h != false || Food.f != false || Food.s != false)
            pthread_cond_wait(&c, &m);

        pthread_mutex_unlock(&m);
    }
}

void* eat(void *args){
    Customer *cust = (Customer *) args;
    while (!stop){
        printf("%c", cust->bring);
    }
}

int main(){
    Customer cust0, cust1, cust2;
    cust0.bring = 'h';
    cust1.bring = 'f';
    cust2.bring = 's';

    Food.h = true;
    Food.f = true;
    Food.s = true;

    pthread_t eat0, eat1, eat2, cook;

    pthread_create(&eat0, NULL, eat,  &cust0);
    pthread_create(&eat1, NULL, eat,  &cust1);
    pthread_create(&eat2, NULL, eat,  &cust2);
    pthread_create(&cook, NULL, chef, NULL);

   
    pthread_mutex_lock(&m);
    Food.h = false;
    Food.f = false;
    Food.s = false;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);

    pthread_join(eat0, NULL);
    pthread_join(eat1, NULL);
    pthread_join(eat2, NULL);

    return 0;
}