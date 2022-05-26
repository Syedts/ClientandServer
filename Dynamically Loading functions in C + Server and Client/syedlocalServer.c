#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <stdbool.h> 
#include <stdlib.h> 


#define LOCAL_SOCKET "localSocket"

int main(int argc, char *argv[], char * envp[]) 
{
    struct sockaddr  x; 

    struct sockaddr_un addr={AF_UNIX,"localSocket"} ;
    char textBuffer[100],msg[100] ;
    int fd,serverSideSocket,bytesRead;
    int bytesSent; 

	FILE * shellOutputFP;  
	char shellInput[100];

    //Create a socket to listen to connections
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) { //Alternative: SOCK_DGRAM, SOCK_RAW
	perror("socket error");
	exit(-1);
    }

    //Create a TCP socket description
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, LOCAL_SOCKET, sizeof(addr.sun_path)-1);

    unlink(LOCAL_SOCKET); //remove the socket file if it already exists

    //Bind the socket to the file.  
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
	perror("bind error");
	exit(-1);
    }

    chmod(LOCAL_SOCKET,0666);  //open up the socket to everyone - not req'd
    perror("chmod"); 
   
    //Listen for an incoming connection
    if (listen(fd, 5) == -1) {
	perror("listen error");
	exit(-1);
    }

    fprintf(stdout,"Local socket server launched\n");	
    //Loop to handle multiple clients one after the other, but not simultaneiously.  
    while (1) {
        fprintf(stdout,"Waiting for a new connection\n");	
	if ( (serverSideSocket = accept(fd, NULL, NULL)) == -1) {
	    perror("accept error");
	    continue;
	}
   
        //Read message from the client and output it to the terminal
	memset(&textBuffer,0,sizeof(textBuffer)); //should clear out prev things

        bytesRead=read(serverSideSocket,textBuffer,sizeof(textBuffer)); //reads in the command

	shellOutputFP=popen(textBuffer,"r"); 

	while(fgets(shellInput,sizeof shellInput,shellOutputFP))
		strncat(msg,shellInput,sizeof msg); 
	
        bytesSent=write(serverSideSocket,msg,strlen(msg)); // sends the result to the Client (server file)
		memset(&msg,0,sizeof(msg));
		pclose(shellOutputFP); 
		
		close(serverSideSocket);
    }


    return 0;
}