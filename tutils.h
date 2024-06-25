#pragma once
#include <stdio.h>
#include <stdlib.h>

const char* pullArg(int *argc, char** argv[]){
	if(*argc <= 0)
		return NULL;
	(*argc)--;
	return *argv[0]++;
}

const char* readFile(const char* filename, long *size){
	FILE *fd = fopen(filename, "r");
	
	fseek(fd, 0, SEEK_END);
	*size = ftell(fd);
	rewind(fd);

	char* ret = malloc(*size+1);
	fread(ret, sizeof(char), *size, fd);
	ret[*size] = 0;
	return ret;
}

int codeChk(int code, const char* context, const char* error(void)){
	if(code != 0){
		if(error != NULL)
			printf("[%s] -> %s \n", context, error());
		else
			printf("[%s]\n", context);
		exit(0);
	}
	return code;
}

void* ptrChk(void* ptr, const char* context, const char* error(void)){
	if(ptr == NULL){
		if(error != NULL)
			printf("[%s] -> %s \n", context, error());
		else
			printf("[%s]\n", context);
		exit(0);
	}
	return ptr;
}
