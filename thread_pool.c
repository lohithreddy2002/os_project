#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_NUM 1

typedef struct Task {
    void (*func)(int,int);
    int arg1,arg2;
} Task;

Task taskQueue[256];
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void executeTask(Task* task) {
    task -> func(task->arg1,task->arg2);
    // int result = task->arg1 + task->arg2;
    // printf("hai");
}

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}



void* startThread(void* args) {
    while (1) {
        Task task;
        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) {
           pthread_cond_wait(&condQueue, &mutexQueue);
        }
        task = taskQueue[0];
        int i;
        for (i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task);
    }
}


void sumfun(int q,int w){
    int sum = q+w;
    printf("number is thread %d,%d and the sum is %d\n",q,w,sum);
}

void product(int q,int w){
    int product = q*w;
    printf("number is thread %d,%d and the product is %d\n",q,w,product);
    }


int main(int argc, char* argv[]) {
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&condQueue, NULL);
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }
    
    for (i = 0; i < 5; i++) {
        Task t = {
            .func = &sumfun,
            .arg1 = rand() % 100,
            .arg2  = rand() % 100
        };
        submitTask(t);
    }

    for (i = 0; i < 5; i++) {
        Task t = {
            .func = &product,
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