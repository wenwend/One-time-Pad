//This program connect to opt_dec_d server and request a decrypt for plaintext.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>


/*get file length and check bad character*/
int getLength(char *filename){
	 
	int fd;
	int i=0;
	int length;
	char *buffer = (char *) calloc(128000, sizeof(char));


	fd=open(filename,O_RDONLY);
	 
	if (fd < 0){
        	fprintf(stderr,"Error: cannot open plaintext file %s\n", filename);
        	exit(1);
   	 }
	
	length=read(fd,buffer,128000); 
	while(i<length){
		if((int)buffer[i]!=' ' && ( buffer[i]< 'A' || buffer[i]>'Z')){
			if(buffer[i]!='\n'){
				 fprintf(stderr,"%s contains invalid characters\n", filename);
				 exit(1);
			}
		}
		i++;
	}
	close(fd);
	 
	return length;
}

void sendMsg(int socket,char *file,int size){
	//fprintf(stderr, "\n\nsending %s\n", file);
	//fprintf(stderr, "\n\nfilelength%d\n", size);
	int fd = open(file,O_RDONLY);
	char buffer[128000];
	memset(buffer,'\0',sizeof(buffer));
	int b_read,b_sent;

	while(size>0){
		b_read=read(fd,buffer,sizeof(buffer));
		if(b_read==0) break;
		if(b_read<0){fprintf(stderr,"otp_enc reading files error\n");exit(1); }
		size-=b_read;
	}
	char *pos;
	pos=buffer;
	while(b_read>0){
		b_sent=send(socket,pos,b_read,0);
		if(b_sent<0){fprintf(stderr,"otp_enc sending files error\n");exit(1);}
		b_read -=b_sent;
		pos +=b_sent;
	}
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[128000];
	int key_l,plaintext_l;
	const char hostname[]="localhost";
	int response;
	
	if(argc!= 4){
		fprintf(stderr,"Usage %s format as <inputfile> <key> <port>\n", argv[0]); exit(1);
    	}
	portNumber=atoi(argv[3]);
	if(portNumber<0 ||portNumber>65535) { fprintf(stderr,"otp_enc invalid port\n");exit(1); }
	key_l=getLength(argv[2]);
	plaintext_l=getLength(argv[1]);
	if(key_l<plaintext_l){
		fprintf(stderr,"otp_enc key %d is too short!\n",argv[2]);exit(1);
	}
	//printf("key size %d, file size %d",key_l,plaintext_l);

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(hostname);
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(1); }
	memcpy((char*) &serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	//Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	char auth[]="dec_ask";
	memset(buffer,'\0',sizeof(buffer));
	//verify the right server!
	send(socketFD,auth,sizeof(auth),0);
	recv(socketFD,buffer,sizeof(buffer),0);
	if(strcmp(buffer,"decrypt_ok")!=0){fprintf(stderr, "CLIENT: Connectiong to wrong server\n"); exit(2);}

	memset(buffer,'\0',sizeof(buffer));
	sendMsg(socketFD,argv[1],plaintext_l);
	sendMsg(socketFD,argv[2],key_l);
	
	response=recv(socketFD,buffer,sizeof(buffer),0);

	if(response<0){fprintf(stderr, "CLIENT: Error response from server\n"); exit(1);}
	printf("%s\n",buffer);


	close(socketFD);
	return 0;
}