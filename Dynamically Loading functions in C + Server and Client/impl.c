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
#include <string.h> 
#include <time.h> 
#include <sys/socket.h>
#include <linux/un.h> 

char *safecat(char * dest, char *src, int maxSize){
  return strncat(dest,src,maxSize-strlen(dest)-1);  
  } 

char * help(char *cmd,char **tokens);

char * timeFunc(char *cmd,char **tokens) 
{
	char temp[1000]; 
       strcat(temp,tokens[0]); 
  time_t now=time(NULL) ;
  setenv("TZ",temp,1); 
  return ctime(&now);    
  } 

char *  prod(char *cmd,char **tokens)
{
	
	float val[1000];
	static float prod = 1.0;
	static char Ans[10000]="";
	int count = 0;
	int i,j;

	memset(Ans,0,sizeof(Ans));
	memset(&prod,0,sizeof(prod));
	memset(val,0,sizeof(val));
 	prod = 1.0;
if(!tokens[1])
{
 return "The product is: 1 and the average is: Undefined"; 	
}
else{
 for( i=0;tokens[i];i++){val[i] = atof(tokens[i]);}
for( j = 0;val[j];j++)
{ 
prod *= val[j];
count++;
}
       
sprintf(Ans," The product is %.2f and the average is : %.2f \n ",prod, prod/count );

  return  Ans ;  
  }
	
  } 



char * files(char *cmd,char **tokens) 
{
  char shellCmd[10000]="ls";
  char shellInput[1000]="";  
  static char result[10000]=""; 
  FILE * shellOutputFP;  
  int i;
	memset(result,0,sizeof(result));
    for(i=0;tokens[i];i++)
     {
       strcat(shellCmd," "); 
       strcat(shellCmd,tokens[i]);  
     }  
   fprintf(stdout,"Verifying the command sent to the shell: %s\n",shellCmd); 
   shellOutputFP=popen(shellCmd,"r"); 
   while(fgets(shellInput,sizeof shellInput,shellOutputFP))   
   safecat(result,shellInput,sizeof result); 
   pclose(shellOutputFP); 
   return result; 
}
char *convert(char *cmd,char **tokens)
{
	struct sockaddr_un addr={AF_UNIX,"localSocket"};
	int clientFD,nBytes;   
	char shellCmd[1000]="units";
  	static char result[10000]=""; 
  	int i;
  	memset(result,0,sizeof(result));
    for(i=0;tokens[i];i++)
     {
       strcat(shellCmd," "); 
       strcat(shellCmd,tokens[i]);  
     }  

  clientFD=socket(AF_UNIX,SOCK_STREAM,0); 

  connect(clientFD,(struct sockaddr *) &addr, sizeof(addr)); 

  write(clientFD,shellCmd,strlen(shellCmd)); 
  
  nBytes=read(clientFD,result, sizeof result) ;

  close(clientFD); 
 return result; 
 } 

struct CMDSTRUCT {
         char *cmd; 
         char *(*method)(); 
	  char *helpMeth;
       } cmdStruct[]={{"help",help,"Available commands: help,time,prod,files,convert"},{"time",timeFunc,"Time: will give you the time of a timezone"},{"prod",prod,"Prod: will give you the product and the average of the numbers"},{"files",files,"files: Will display all the files in the directory"},{"convert",convert,"Convert: use double quotes to put in your conversion ex:  \"feet\" to \"meters\" "}, {NULL,NULL,NULL}} ;   
   

char * help(char *cmd,char **tokens){ 
	fprintf(stdout,"help \n"); 	
	static char result[1000]="";
	int i,j;
 	if (tokens[0]==NULL){
		return ("Available commands: help,time,prod,files,convert");
	}else{					
		for(i=0;tokens[i];i++){			
			for(j=0;cmdStruct[j].cmd;j++){			
				if(strcasecmp(tokens[i],cmdStruct[j].cmd) == 0){					
					sprintf(result,"%s",cmdStruct[j].helpMeth); 									
					break;
				}	
			}				
		}	
			return result;		
	}
	
	return "help"; 
}
