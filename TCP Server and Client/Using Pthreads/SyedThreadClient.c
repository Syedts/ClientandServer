#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h> 
#include <readline/readline.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT "40167" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

 	 pthread_t thread2;

//int sockfd;
  

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int conSoc(char *address)
{
	int sockfd,rv;
	struct addrinfo hints, *servinfo, *p;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

 	if ((rv = getaddrinfo(address, PORT, &hints, &servinfo)) != 0) 
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}


      for(p = servinfo; p != NULL; p = p->ai_next) {
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
	    perror("client: socket");
	    continue;
	}

	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	    close(sockfd);
	    perror("client: connect");
	    continue;
	}

	break;
    }

    if (p == NULL) 
	{
	fprintf(stderr, "client: failed to connect\n");
	exit(1);
	} 


 inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	    s, sizeof s);
    printf("client: connecting to %s\n", s);

	return sockfd;		
}


void *receiveFunc(void * arg)
{
	int *sockfd = (int *) arg;
	int numbytes;
char incoming[1000];

while(true)
{
	//Receive message from server 
  	   if ((numbytes = recv(*sockfd, incoming, sizeof incoming -1 , 0)) == -1) {  
		    perror("recv");
		    break;
		}
            fprintf(stdout,"Msg from server: %s\n",incoming);	 
}
//close(sockfd);
}



/*

void *sendFunc(void * arg)
{
char *msg=readline("Send msg to server: "); 
       if ( msg) {
          if( numbytes=send(sockfd, msg,strlen(msg)+1, 0) == -1) {
               perror("send: Server probably quit"); 
               break;
               }
            } 
close(sockfd);
}
*/


int main(int argc, char *argv[])
{
    int sockfd;  
    char buf[MAXDATASIZE];
       
    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
	sockfd = conSoc(argv[1]);

    //char incoming[1000];  
    int numbytes; 
int i ;
	 pthread_t thread1;
	 if(pthread_create(&thread2,NULL, receiveFunc,(void *)&sockfd))perror("failed to create thread");

	if(pthread_create(&thread1,NULL, receiveFunc,(void *)&sockfd))perror("failed to create thread");
	
while(true) 
{
	
		char *msg=readline("Send msg to server: "); 
       if ( msg) {
          if( numbytes=send(sockfd, msg,strlen(msg)+1, 0) == -1) {
               perror("send: Server probably quit"); 
               break;
               }
            } 
		
}
      close(sockfd); 
  } 
