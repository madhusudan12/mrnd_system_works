// ConcurrentQueue.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "client.h"

int main(int argc, _TCHAR* argv[])
{

	concInit();

	pthread_t t1, t2;

	pthread_create(&t1, NULL, createUserTask, NULL);
	pthread_create(&t2, NULL, getUserDataTask, NULL);


	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("\ndone....");
	return 0;
}

