


#include<string.h>
#include<stdlib.h>
#include<Windows.h>

struct Database{
	int key;
	char username[32];
};


struct Database db[100];
int dbCount;

int insertUser(int key, char *username){
	for (int i = 0; i < dbCount; i++){
		if (key == db[i].key){
			return -1;
		}
	}
	db[dbCount].key = key;
	strcpy(db[dbCount].username, username);
	dbCount++;
	return 1;
}

char *getData(int key){
	char *uName = (char *)calloc(1, sizeof(char));
	for (int i = 0; i < dbCount; i++){
		if (key == db[i].key){
			strcpy(uName, db[i].username);
			return uName;
		}
	}
	return NULL;
}