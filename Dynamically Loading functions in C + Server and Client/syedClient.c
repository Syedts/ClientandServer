#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <readline/readline.h>
#include <readline/history.h> 
#include <stdbool.h> 
#include <signal.h> 

void psignalHandler(int signum)
{
  fprintf(stderr,"Broken pipe server not available\n"); 
  exit(1); 

  } 

void ctrlCHandler(int signnum)
{
   fprintf(stdout,"Program ended using <ctrl>C\n"); 
   exit(2); 
 } 

int main(int argc, char * argv[],char * envp[])
{
  char incomingMsg[10000]; 
  int  nBytes; //size of msg rec'd

 
  signal(SIGPIPE,psignalHandler); 
  signal(SIGINT,ctrlCHandler); 

  int SyedPipeIn=open("SyedPipeIn",O_WRONLY);
  int SyedPipeOut=open("SyedPipeOut",O_RDONLY);
  
 read_history(".rlDemo.history"); 

  char *msg, *expansion; 
	char ** tokens;
  while(true) {

	 history_comment_char='#'; 
          //Send a message to the server
          msg=readline("Enter a Command: "); 
          if(!msg) { fprintf(stdout,"Client has ended the conversation using <ctrl>D\n");
                     close(SyedPipeOut); 
                     close(SyedPipeIn) ;
                    } 

          write(SyedPipeIn,msg,strlen(msg));         
	  memset(incomingMsg,0,sizeof incomingMsg); 
		history_expand(msg,&expansion);
	 fprintf(stdout,"Syed Shahid  %s\n",expansion); 
		add_history(expansion); 
		
		if(tokens=history_tokenize(expansion)) 
	for(int i=0;tokens[i];i++)
	  	{
	  		fprintf(stdout,"%d) %s\n",i, tokens[i]); 
	  	} 

          //Get and display the server's response
          memset(incomingMsg,0,sizeof incomingMsg); 
          nBytes=read(SyedPipeOut,incomingMsg,sizeof incomingMsg); 
          if(!nBytes) { fprintf(stdout,"Server has closed the connection\n"); break; } 
          fprintf(stdout, "Server said: %*s \n \n",nBytes,incomingMsg);  

        }  
		write_history(".rlDemo.history"); 
		system("echo Your session history is; cat -n .rlDemo.history");  
  fprintf(stdout,"Client shutting down\n"); 
  return 0;
  }
