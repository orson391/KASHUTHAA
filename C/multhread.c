#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <SDL2/SDL_net.h>

#define MAX_THREADS 4  // Number of worker threads
#define MAX_TASKS 10   // Number of tasks

// Task queue
typedef struct {
    void (*function)(void *);  // Task function
    void *arg;                 // Arguments for the function
} Task;

Task taskQueue[MAX_TASKS];
int taskCount = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_t threads[MAX_THREADS];

// Function for worker threads
void *workerThread(void *arg) {
    while (1) {
        Task task;

        // Lock the task queue
        pthread_mutex_lock(&mutex);

        // Wait until there are tasks in the queue
        while (taskCount == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Get the task from the queue
        task = taskQueue[--taskCount];

        // Unlock the queue
        pthread_mutex_unlock(&mutex);

        // Execute the task
        task.function(task.arg);
    }
    return NULL;
}

// Function to add a task to the queue
void addTask(void (*function)(void *), void *arg) {
    pthread_mutex_lock(&mutex);

    if (taskCount < MAX_TASKS) {
        taskQueue[taskCount].function = function;
        taskQueue[taskCount].arg = arg;
        taskCount++;
    }

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}

// Sample task function
void printTask(void *arg) {
    int taskNum = *(int *)arg;
    printf("Task %d is being processed by thread %ld\n", taskNum, pthread_self());
    sleep(1);
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create worker threads
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, workerThread, NULL);
    }

    // Add tasks
    for (int i = 0; i < MAX_TASKS; i++) {
        int *taskNum = malloc(sizeof(int));
        *taskNum = i;
        addTask(printTask, taskNum);
        sleep(1);
    }

    // Wait for threads to finish (optional)
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}
