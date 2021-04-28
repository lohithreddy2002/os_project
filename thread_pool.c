#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_NUM 1    //number of threads 

typedef struct Task {       //task to be submiteed by the thread
    void (*func)(int,int);
    int arg1,arg2;
} Task;

Task taskQueue[250];     //number of tasks in the queue
int taskCount = 0;       //count of the present task

pthread_mutex_t mutexQueue;     // mutex
pthread_cond_t condQueue;       // conditional variable

void executeTask(Task* task) {                     // function that executes task in the thread
    task -> func(task->arg1,task->arg2);   
    // int result = task->arg1 + task->arg2;
    // printf("hai");
}

void submitTask(Task task) {                     //function that submits the task to the queue
    pthread_mutex_lock(&mutexQueue);             //mutex acquire lock
    taskQueue[taskCount] = task;                 // put the task in the task queue
    taskCount++;                                 // increase the count variable
    pthread_mutex_unlock(&mutexQueue);           // muntex release lock
    pthread_cond_signal(&condQueue);             // conditional signal pointing to conditional wait in start thread
}



void* startThread(void* args) {                         //function stating the thread
    while (1) {  
        Task task;
        pthread_mutex_lock(&mutexQueue);              //mutex acquire 
        while (taskCount == 0) {                          
           pthread_cond_wait(&condQueue, &mutexQueue); //conditional wait 
        }
        task = taskQueue[0];      
        int i;
        for (i = 0; i < taskCount - 1; i++) {        //push all the elements in the queue to the left
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;                                 //decrease the taskcount
        pthread_mutex_unlock(&mutexQueue);           //mutex release
        executeTask(&task);                          // calling execute task function
    }
}

//function we want to execute on the thread
void sumfun(int q,int w){
    int sum = q+w;
    printf("number is thread %d,%d and the sum is %d\n",q,w,sum);
}

void product(int q,int w){
    int product = q*w;
    printf("number is thread %d,%d and the product is %d\n",q,w,product);
    }

//end

int main(int argc, char* argv[]) {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);          //intitalize mutex
    pthread_cond_init(&condQueue, NULL);            //initialize coditional variable
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {    //creating threads and checking if created
            perror("Failed to create the thread");
        }
    }
    
    for (i = 0; i < 50; i++) {
        Task t = {
            .func = &sumfun,
            .arg1 = rand() % 100,
            .arg2  = rand() % 100
        };
        submitTask(t);
    }


    


    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    return 0;
}