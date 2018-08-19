

#include<pthread.h>
#include "database.h"
#include<stdio.h>
#define FREE 1
#define BUSY 2
#define CREATE_USER 1
#define VIEW_USER 2


struct RequestObject{
	int type;
	int key;
	char *username;
	int status;
	//void *funcPtr;
};

struct Queue{
	struct RequestObject *requestObject;
	struct Queue *next;

};

struct ThreadData{
	int status;
	struct RequestObject *requestObject;
};


struct ConcurrentQueue{
	struct ThreadData thread[4];
	struct Queue *head;
	struct Queue *tail;
};



struct ConcurrentQueue concurrentQueue;

pthread_rwlock_t rwLock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t rwLockInsert = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t insertMutexLock = PTHREAD_MUTEX_INITIALIZER;

int anyRequestInQueue(){
	
	if (concurrentQueue.head == NULL){
		return -1;
	}
		
	return 1;
}


void addToQueue(struct RequestObject *object){

	//pthread_mutex_lock(&insertMutexLock);
	pthread_rwlock_wrlock(&rwLockInsert);
	printf("\n%d in for   %d...", object->key,object->type);

	if (concurrentQueue.head == NULL){
		concurrentQueue.head = (struct Queue *)calloc(1, sizeof(struct Queue));
		concurrentQueue.head->requestObject = object;
		concurrentQueue.head->next = NULL;
		concurrentQueue.tail = concurrentQueue.head;
	}
	concurrentQueue.tail->next = (struct Queue *)calloc(1, sizeof(struct Queue));
	concurrentQueue.tail = concurrentQueue.tail->next;
	concurrentQueue.tail->requestObject = object;
	concurrentQueue.tail->next = NULL;

	//pthread_mutex_unlock(&insertMutexLock);
	pthread_rwlock_unlock(&rwLockInsert);
}



int findFreeThread(){
	for (int i = 0; i < 4; i++){
		if (concurrentQueue.thread[i].status == FREE){
			return i;
		}

	}
	return -1;
}


void *scheduler(void *args){

	while (1){
		//pthread_mutex_lock(&insertMutexLock);
		pthread_rwlock_wrlock(&rwLockInsert);
		int isQueueEmpty = anyRequestInQueue();
		if (isQueueEmpty != -1){
			int threadIndex = findFreeThread();
			if (threadIndex != -1){
				
				//printf("\nhii..");
				//printf("\nassigning %d (%d )task to %d", concurrentQueue.thread[threadIndex].requestObject->key, concurrentQueue.thread[threadIndex].requestObject->type,threadIndex);
				concurrentQueue.thread[threadIndex].requestObject = concurrentQueue.head->requestObject;
				struct Queue *temp = concurrentQueue.head;
				concurrentQueue.head = concurrentQueue.head->next;
				free(temp);
			}
		}
		//pthread_mutex_unlock(&insertMutexLock);
		pthread_rwlock_unlock(&rwLockInsert);
	}
	return NULL;
}



void *service(void *args){
	int *i = (int *)args;
	int index = *i;
	while (1){
		if (concurrentQueue.thread[index].requestObject != NULL){

			concurrentQueue.thread[index].status = BUSY;
			printf("\nthread %d proesing %d for %d", index, concurrentQueue.thread[index].requestObject->key, concurrentQueue.thread[index].requestObject->type);

			if (concurrentQueue.thread[index].requestObject->type == CREATE_USER){

				pthread_rwlock_wrlock(&rwLock);


				concurrentQueue.thread[index].requestObject->status = insertUser(concurrentQueue.thread[index].requestObject->key, concurrentQueue.thread[index].requestObject->username);
				if (concurrentQueue.thread[index].requestObject->status == -1)
					printf("\n%d key already exist...", concurrentQueue.thread[index].requestObject->key);
				else
					printf("\n%d record inserted", concurrentQueue.thread[index].requestObject->key);


				pthread_rwlock_unlock(&rwLock);
			}
			else{

				pthread_rwlock_rdlock(&rwLock);

				concurrentQueue.thread[index].requestObject->username = getData(concurrentQueue.thread[index].requestObject->key);
				if (concurrentQueue.thread[index].requestObject->username == NULL)
					printf("\n\t\t\t\t%d  key doesn't exist...", concurrentQueue.thread[index].requestObject->key);
				else
					printf("\n\t\t\t\t%d    %s", concurrentQueue.thread[index].requestObject->key, concurrentQueue.thread[index].requestObject->username);

				pthread_rwlock_unlock(&rwLock);

			}

			concurrentQueue.thread[index].requestObject = NULL;
			concurrentQueue.thread[index].status = FREE;

		}
	}
	return NULL;
}





void createThreads(){
	pthread_t tp[4];
	int *j;
	for (int i = 0; i < 4; i++){
		j = (int *)malloc(sizeof(int));
		j[0] = i;
		pthread_create(&tp[i], NULL, service, (void *)j);
		concurrentQueue.thread[i].status = FREE;
		concurrentQueue.thread[i].requestObject = NULL;
	}
}


void concInit(){

	createThreads();

	pthread_t schedulerThread;

	pthread_create(&schedulerThread, NULL, scheduler, NULL);
}