

#include "server.h"

int max = 100;
int c = 0;
int key = 1;




void *createUserTask(void *args){

	struct RequestObject *obj;
	while (c<100){
		obj = (struct RequestObject *)calloc(1, sizeof(struct RequestObject));
		obj->key = c;
		obj->username = (char *)calloc(32, sizeof(char));
		obj->username = "aaa";
		obj->type = CREATE_USER;


		c++;
		addToQueue(obj);



		Sleep(10);
	}


	return NULL;
}

void *getUserDataTask(void *args){
	struct RequestObject obj;;
	while (key<100){
		obj.key = key;
		obj.type = VIEW_USER;
		key++;


		addToQueue(&obj);


		Sleep(30);

	}

	return NULL;
}