#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h> 
#include <time.h>
#include <unistd.h>

volatile int counter = 0;
int MAX = 100;

char* CHECK = "\u2713";
char* CROSS = "\u2717";

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wakeChef = PTHREAD_COND_INITIALIZER;
pthread_cond_t wakeHCust = PTHREAD_COND_INITIALIZER;
pthread_cond_t wakeFCust = PTHREAD_COND_INITIALIZER;
pthread_cond_t wakeSCust = PTHREAD_COND_INITIALIZER;

typedef struct{
    int id;
    char bring;
    int fedCount;
    bool gotH, gotF, gotS;
} Customer;

struct{
    volatile bool h, f, s;
} food;

void printCust(Customer *cust){
    printf("\t| h%s ", (cust->gotH ? CHECK: CROSS));
    printf("f%s ", (cust->gotF ? CHECK: CROSS));
    printf("s%s\n", (cust->gotS ? CHECK: CROSS));
}

void printFood(){
    printf("\nROUND %d | hamburger%s ", counter, (food.h ? CHECK: CROSS));
    printf("fries%s ", (food.f ? CHECK: CROSS));
    printf("soda%s\n", (food.s ? CHECK: CROSS));
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
    item1 = rand() % 3; 
    do{
        srand(time(0));
        item2 = rand() % 3;
    } while(item1 == item2); 
    setItem(item1);
    setItem(item2);
}


void* chef(void *arg){
    while (counter != MAX){
        pthread_mutex_lock(&m);
        printf("chef wake\n");
        makefood();
        printFood();
        counter++;
        if(food.h == true && food.f == true)
            pthread_cond_signal(&wakeSCust);
        else if(food.h == true && food.s == true)
            pthread_cond_signal(&wakeFCust);
        else
            pthread_cond_signal(&wakeHCust);

        while (food.h != false || food.f != false || food.s != false){
            printf("chef sleep\n");
            pthread_cond_wait(&wakeChef, &m);
        }
        pthread_mutex_unlock(&m);
    }
    pthread_cond_signal(&wakeSCust);
    pthread_cond_signal(&wakeFCust);
    pthread_cond_signal(&wakeHCust);
    //stop = true;
}


void pick(Customer * cust){
    if(cust->gotH != true && food.h == true){
        cust->gotH = true;
        food.h = false;
        printf("Cust %d picked hamburger", cust->id); printCust(cust);
    } 
    
    if(cust->gotF != true && food.f == true){
        cust->gotF = true;
        food.f = false;
        printf("Cust %d picked fries", cust->id); printCust(cust);
    }
    
    if(cust->gotS != true && food.s == true){
        cust->gotS = true;
        food.s = false;
        printf("Cust %d picked soda", cust->id); printCust(cust);
    }
}

void fed(Customer *cust){
    cust->fedCount++;
    switch(cust->bring){
        case 'h': 
            cust->gotF = false; 
            cust->gotS = false; 
            break;
        case 'f':
            cust->gotH = false;
            cust->gotS = false;
            break;
        case 's':
            cust->gotH = false;
            cust->gotF = false;
            break;
    }
    printf("Cust %d fed\t", cust->id);
    printCust(cust);
    //sleep(10);
}

void* eat(void *args){
    Customer *cust = (Customer *) args;
    while (true){

        pthread_mutex_lock(&m);

        if(counter >= MAX){
            pthread_mutex_unlock(&m);
            break;
        }

        printf("Cust %d sleeps\n", cust->id);
        switch(cust->bring){
            case 'h': pthread_cond_wait(&wakeHCust, &m); break;
            case 'f': pthread_cond_wait(&wakeFCust, &m); break;
            case 's': pthread_cond_wait(&wakeSCust, &m); break;
        }

        

        pick(cust);

               
        if(food.h == false && food.f == false && food.s == false){
            pthread_cond_signal(&wakeChef);
        }           

        

        if(cust->gotH == true && cust->gotF == true && cust->gotS == true){
            fed(cust);
            //pthread_cond_wait(&foodMade, &m);
        }
        
        pthread_mutex_unlock(&m);
    }
}

int main(){
    Customer cust1 = (Customer){.id = 1, .bring = 'h', .fedCount = 0, .gotH = true, .gotF = false, .gotS = false};
    Customer cust2 = (Customer){.id = 2, .bring = 'f', .fedCount = 0, .gotH = false, .gotF = true, .gotS = false};
    Customer cust3 = (Customer){.id = 3, .bring = 's', .fedCount = 0, .gotH = false, .gotF = false, .gotS = true};

    food.h = false;
    food.f = false;
    food.s = false;

    pthread_t eat1, eat2, eat3, cook;
 
    pthread_create(&eat1, NULL, eat,  &cust1);
    pthread_create(&eat2, NULL, eat,  &cust2);
    pthread_create(&eat3, NULL, eat,  &cust3);
    pthread_create(&cook, NULL, chef, NULL);

   
    // pthread_mutex_lock(&m);
    // food.h = false;
    // food.f = false;
    // food.s = false;
    // pthread_cond_signal(&c);
    // pthread_mutex_unlock(&m);

    pthread_join(eat1, NULL);
    pthread_join(eat2, NULL);
    pthread_join(eat3, NULL);
    pthread_join(cook, NULL);


    printf("\n\nFed count:\n");
    printf("Cust %d: %d\n", cust1.id, cust1.fedCount);
    printf("Cust %d: %d\n", cust2.id, cust2.fedCount);
    printf("Cust %d: %d\n", cust3.id, cust3.fedCount);


    return 0;
}