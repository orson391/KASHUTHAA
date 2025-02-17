// KazhuthaServer.cpp : Defines the entry point for the application.
//

#include "KazhuthaServer.h"
/*
//multithreding things
#define MAX_THREADS 4  // Number of worker threads
#define MAX_TASKS 10   // Number of tasks
typedef struct {
	void (*function)(void*);  // Task function
	void* arg;                 // Arguments for the function
} Task;


void* workerThread(void* arg) {
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


void addTask(void (*function)(void*), void* arg) {
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
void printTask(void* arg) {
	int taskNum = *(int*)arg;
	printf("Task %d is being processed by thread %ld\n", taskNum, pthread_self());
	sleep(1);
}
*/





//server things
#define MAX_CLIENTS 10
#define BUFFER_SIZE 512
#define PORT 12345

//void intialize_sdl_net(IPaddress ip,TCPsocket server)







int main()
{
		/*
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);

		// Create worker threads
		for (int i = 0; i < MAX_THREADS; i++) {
			pthread_create(&threads[i], NULL, workerThread, NULL);
		}
		*/
	
		printf("Hello KazhuthaServer.c\n--------------------\n");

		if (SDLNet_Init() < 0)
		{
			printf("SDLNet Initialization Error: %s\n", SDLNet_GetError());
			return 1;
		}
		printf("SDLNet Initialized Sucessfully\n\n");
		IPaddress ip;
		if (SDLNet_ResolveHost(&ip, NULL, PORT) < 0) {
			printf("SDLNet Resolve Host Error: %s\n", SDLNet_GetError());
			return 1;
		}
		printf("SDLNet Resolve Host Sucessfully\n\n");

		TCPsocket server = SDLNet_TCP_Open(&ip);
		if (!server) {
			printf("SDLNet TCP Open Error: %s\n", SDLNet_GetError());
			return 1;
		}
		printf("SDLNet TCP Open SucessFull\n\n");
	

	printf("Server started on port: %d ...\n----------------------\n", PORT);


	SDLNet_SocketSet socket_set = SDLNet_AllocSocketSet(MAX_CLIENTS + 1);
	SDLNet_TCP_AddSocket(socket_set, server);


	return 0;
}
