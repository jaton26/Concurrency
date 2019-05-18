#include <stdio.h>
#include <stdbool.h>
#include <pthread.h> 


typedef struct{
    char bring;
    int fedCount;
    bool gotH, gotF, gotS;
    pthread_mutex_t *lock;

} Customer;

typedef struct{
    bool h, f, s;
} Food;


void* chef(void *arg){
    int counter = 0;
    
}

void* eat(void *args){
    Customer *cust = (Customer *) args;
    
}

int main(){
    Customer cust0, cust1, cust2;
    cust0.bring = 'h';
    cust1.bring = 'f';
    cust2.bring = 's';

    pthread_t eat0, eat1, eat2, cook;

    pthread_create(&eat0, NULL, eat,  &cust0);
    pthread_create(&eat1, NULL, eat,  &cust1);
    pthread_create(&eat2, NULL, eat,  &cust2);
    pthread_create(&cook, NULL, chef, NULL);

    pthread_join(eat0, NULL);
    pthread_join(eat1, NULL);
    pthread_join(eat2, NULL);

    return 0;
}