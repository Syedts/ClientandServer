#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdbool.h> 
#include <readline/readline.h>
#include <pthread.h>

#define PORT "40"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

//Global Variable 




void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void clientDisconnects(int signum)
{

  fprintf(stdout,"broken pipe: disconnected\n"); 
 } 

 

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int crt_soc(char *port)
{
	
	 	int sockfd,rv;
	 	struct addrinfo hints, *servinfo, *p;
		struct sigaction sa;



 	memset(&hints, 0, sizeof hints);
   	 hints.ai_family = AF_UNSPEC; //Could come back either AF_INET or AF_INET6
    	hints.ai_socktype = SOCK_STREAM; //This server will use TCP not UDP
   	 hints.ai_flags = AI_PASSIVE; // use my IP


	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	 {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
   	 }

	 for(p = servinfo; p != NULL; p = p->ai_next) 
			{
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server1: socket");
            continue;
        }
	
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server1: bind");
            continue;
        }
        break;
		}
	if (p == NULL)  {
        fprintf(stderr, "server1: failed to bind\n");
        exit(2);
   		}

	freeaddrinfo(servinfo);
 if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

   

	return sockfd;
}



void *rec_fucntion(void * arg)
{
int *new_fd = (int *) arg;
int numbytes;
char incoming[1000];


while(true) {

  	   if ((numbytes = recv(*new_fd, incoming, sizeof incoming -1 , 0)) == -1) {
		    perror("recv");
		    break;
		}
            fprintf(stdout,"Msg from client: %s numbytes: %d\n",incoming,numbytes);	 
if(strcasecmp(incoming,"bye")==0) break;
            
}
	 fprintf(stdout,"Disconnecting from client fd: %d\n",*new_fd);


}


int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
     struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
	int numbytes;

    struct sockaddr_in ip4;
    struct sockaddr_in6  ip6; 

    
    fprintf(stdout,"Hello, I'm pid %d listening on port %s using TCP.\n",getpid(),PORT);	
   
 
	sockfd = crt_soc("400");

	sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
   
    signal(SIGPIPE,clientDisconnects); 
  
    fprintf(stdout,"Server setup complete\n");

    while(true)
	 {  
		sin_size = sizeof their_addr;
	
        //Create a 2nd socket that will be used to talk to the client
        fprintf(stdout,"Server is waiting for a new client\n");	
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);

        printf("server1: got connection from %s talking on fd: %d\n",s,new_fd);  

	 pthread_t myThread[10];  // NEED TO CHANGE THIS 
	int i;
	for(i = 0; i<10 ; i++)
		{        
			if(pthread_create(&myThread[i],NULL, rec_fucntion,(void *)&new_fd))perror("failed to create thread");
		}

while(true){
  char *msg=readline("Respond to client: "); 
            if ( msg) {
               if( numbytes=send(new_fd, msg,strlen(msg)+1, 0) == -1) {
                          perror("send: Client probably quit"); //wait for a new client
                          break;
                          }
            } 

}
	

    	} 
    fprintf(stdout,"Ending program\n");	
	close(new_fd);
    return 0;
}
