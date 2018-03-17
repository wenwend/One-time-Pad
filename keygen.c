//This program creates a key file of specified length

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]){
	
	//seed random
	time_t sysClock;
 	srand((unsigned) time(&sysClock));

	//first check error
	if(argc != 2)
		{
		fprintf(stderr,"Usage: %d input\n",argv[0]);
		exit(1);
	}

	int i,length=atoi(argv[1]);
	char key[length+1];
	char letter;

	for(i=0;i<length;i++){
		letter=" ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand()%27];
		key[i]=letter;
	}
	key[length]='\0';
	printf("%s\n",key);
	

	return 0;
}