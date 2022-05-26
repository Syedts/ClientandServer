
/*
   File: Q2.c
   Course: ceng 320
   Author: Syed Shahid Syed Yacoob
   Date: Thursday Oct 01, 2020   21:59 PM
*/




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
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h> 
#include <readline/readline.h>
#include <readline/history.h> 
#include <pthread.h>

#define PORT "40167"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold
#define MAX 50
//Global Variable 

//int new_fd;


// This structure records all client 
struct clientRec{
	long fd;
	char * port;
	char * username;
	char *ipaddr;
}clientRec[MAX]={{0,"\0","\0","\0"}};


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
//Commands 
char * login(char *cmd,char **tokens )   
{
int i; 
memset(clientRec[i].username,0,sizeof (clientRec[i].username));
char newname[]= "Client:  ";
strcat(newname,tokens[0]);
clientRec[i].username = newname;

  return clientRec[i].username;    
  } // end of login commmand




char * logout(char *cmd,char **tokens) 
{ 
	int i;
	//memset(clientRec[i].fd,0,sizeof(clientRec[i].fd));
	memset(clientRec[i].port,0,sizeof(clientRec[i].port));
	memset(clientRec[i].username,0,sizeof(clientRec[i].username));
	memset(clientRec[i].ipaddr,0,sizeof(clientRec[i].ipaddr));
	pthread_exit(0); // this exits out of the child thread
  	return "Client Data Cleared and Logged out";    
} 
  
  char * list(char *cmd,char **tokens) 
{
	int i;
	static char Listresult[1000]="";  // static so it will store the result 
memset(Listresult,0,sizeof(Listresult));

	for(i = 0 ; clientRec[i].username; i++)
{
	if(strcasecmp(clientRec[i].username, "\0") != 0)
{
	strcat(Listresult, clientRec[i].username);
	strcat(Listresult," ");
}
}

  return Listresult;    
} 
  
  
  char * privateusrmsg(char *cmd,char **tokens) 
{
	

  return "d is recognized";    
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
} // end of our socket connection function 






struct CMDSTRUCT {
         char *cmd; 
         char *(*method)(); 
       } cmdStruct[]={{"login",login},{"logout",logout},{"list",list},{"private",privateusrmsg}, {NULL,NULL}} ;   

char *interpret(char *cmdline)
{
  char **tokens;
  char *cmd;
  int  i; 
  char *result; 

  tokens=history_tokenize(cmdline);   //Split cmdline into individual words.  
  if(!tokens) return "no response needed"; 
  cmd=tokens[0]; 


  for(i=0;cmdStruct[i].cmd;i++)
    if(strcasecmp(cmd,cmdStruct[i].cmd)==0) return (cmdStruct[i].method)(cmd,&tokens[1],cmdline);    

  
 
  return "command not found"; 

 } 

void *receiveFunc(void * arg)
{
int *new_fd = (int *) arg;
int numbytes;
char incoming[1000];


while(true) {
 //Receive message from client
           //If client disconnects while server is waiting this will generate a SIGPIPE signal
  	   if ((numbytes = recv(*new_fd, incoming, sizeof incoming -1 , 0)) == -1) {
		    perror("recv");
		   pthread_exit(0);
		}
            fprintf(stdout,"Msg from client: %s numbytes: %d\n",incoming,numbytes);	 
if(strcasecmp(incoming,"bye")==0) pthread_exit(0);
            
			char *response=interpret(incoming);
			
		//	send(clientRec[Clientcount].fd, response,strlen(response)+1, 0);//  HAVING TROUBLE SENDING BACK RESULT TO THE CLIENT SO I AM PRONTING IT TO THE SERVER FOR NOW 
			 fprintf(stdout,"%s\n",response);

	}
	 fprintf(stdout,"Disconnecting from client fd: %d\n",*new_fd);
pthread_exit(0);

}



 





int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    char *msg, *expansion; 
	char ** tokens;
	int numbytes;
	char incoming[1000];
	int Clientcount = 0;
	char *cmdLine; 
 
    struct sockaddr_in ip4;
    struct sockaddr_in6  ip6; 

    
    fprintf(stdout,"Hello, I'm pid %d listening on port %s using TCP.\n",getpid(),PORT);	
   
 
	sockfd = crt_soc("40167");

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
	 {  // main accept() loop
      sin_size = sizeof their_addr;
	char username[30];

        //Create a 2nd socket that will be used to talk to the client
        fprintf(stdout,"Server is waiting for a new client\n");	
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }


        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);

        printf("server1: got connection from %s talking on fd: %d\n",s,new_fd);  
	fprintf(stdout,"The commands for this program are: a) login username  b) logout c) list d) private   \n");

	 pthread_t myThread;

	if(pthread_create(&myThread,NULL, receiveFunc,(void *)&new_fd))perror("failed to create thread");
	

	sprintf(username,"Client # %d",Clientcount);
	clientRec[Clientcount].fd = new_fd;
	clientRec[Clientcount].port = PORT;
	clientRec[Clientcount].username = username;
	clientRec[Clientcount].ipaddr = s;

	Clientcount++;

    	} 
    fprintf(stdout,"Ending program\n");	
	close(new_fd);
    return 0;
}
