//This serves as the server side for socket, it will encrypt the message client sent.
//It will have control socket and data socket

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
int charToInt (char c){
	if (c == ' ')
		return 26;
	else
		return (c - 'A');
}

char intToChar(int i){
	if (i == 26){
		return ' ';
	}
	else {
		return (i + 'A');
	}
}
void encrypt(char plaintext[], char key[]){
	//fprintf(stderr,"encrypt(%.100s, %.100s)\n", plaintext, key);
	int i;
	int n;
	for(i=0;plaintext[i]!='\n';i++){
		//char ch=plaintext[i];
		n=(charToInt(plaintext[i]) + charToInt(key[i])) % 27;
		//printf("plaintext[i] = %c, key[i] = %c\n", plaintext[i], key[i]);
		plaintext[i]=intToChar(n);
	}
	plaintext[i]='\0';
	//fprintf(stderr,"after encrypt(%.100s)\n", plaintext);
}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber,status;
	socklen_t sizeOfClientInfo;
	char buffer[128000];
	struct sockaddr_in serverAddress, clientAddress;
	pid_t pid;
	int size,key_l,plaintext_l;

	if (argc != 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
	
	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");
	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	

	while (1) {
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		
		//fork
		pid = fork();
		if(pid<0){error("Server:ERROR forking!");exit(1);}
		//child
		if(pid==0){
			//printf("at child process\n");
			memset(buffer,'\0',sizeof(buffer));
			int b_size=sizeof(buffer);
			int b_get=0;
			int i,line=0;
			char *pos=buffer;
			char *keyPos;

			recv(establishedConnectionFD,buffer,sizeof(buffer),0);
			
			if(strcmp(buffer,"enc_ask_right")!=0){
				fprintf(stderr,"invalid client\n");
				char response[]="ERROR";
				send(establishedConnectionFD,response,sizeof(response),0);
				exit(2);
			}
			else{
				char response[]="enc_rep_right";
				send(establishedConnectionFD,response,sizeof(response),0);
			}

			memset(buffer,'\0',sizeof(buffer));
			while(1){
				b_get=recv(establishedConnectionFD,pos,b_size,0);
				if(b_get<0){ fprintf(stderr,"SEVER data receving error\n"); exit(1); }
				if(b_get==0) break;
				for(i=0;i<b_get;i++){
					if(pos[i]=='\n'){
						line++;
						if(line==1) keyPos=pos+i+1; //now keyPos is key text
					}
				}
			

				if(line==2) break;
				pos +=b_get;
				b_size -=b_get;
			}
			char plaintext[128000];
			memset(plaintext,'\0',sizeof(plaintext));
			strncpy(plaintext,buffer,keyPos-buffer);
			//printf("encrypt SERVER: message before encrypt %s \n", plaintext);
			encrypt(plaintext,keyPos);
			//printf("encrypt SERVER: message after encrypt %s\n", plaintext);
			send(establishedConnectionFD,plaintext,strlen(plaintext),0);

		}
		close(establishedConnectionFD);
	
		if(pid>0){
			//printf("at parents process\n");
			pid = waitpid(-1, &status, WNOHANG);
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}

	close(listenSocketFD); // Close the listening socket
	return 0;
}


