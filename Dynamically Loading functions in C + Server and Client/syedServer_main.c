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

//Socket related includes
#include <sys/socket.h>
#include <linux/un.h> 

#include <dlfcn.h>

char *intepret(char *cmdline);
char * help(char *cmd,char **tokens);

void psignalHandler(int signum)
{
  fprintf(stderr,"Broken pipe - client not available\n");  
  exit(1); 
  }  
void ctrlCHandler(int signum)
{
  fprintf(stderr,"Command server terminated using <ctrl>C\n");  
  exit(1); 
  } 


char *safecat(char * dest, char *src, int maxSize) 
{
  return strncat(dest,src,maxSize-strlen(dest)-1);  
  } 



char * help(char *cmd,char **tokens)
{
  return "This is help from the main program"; 
  
  } 
char * timeFunc(char *cmd,char **tokens){
	return "This is time from the main program"; }

char *  prod(char *cmd,char **tokens)
{
  return "This is prod from the main program";  
  } 
char * files(char *cmd,char **tokens) 
{
return "This is files from the main program"; 
}

char *convert(char *cmd,char **tokens)
{
  return "This is convert from the main program";
 } 





 

struct CMDSTRUCT {
         char *cmd; 
         char *(*method)(); 
	  char *helpMeth;
       } cmdStruct[]={{"help",help,"Available commands: help,time,prod,files,convert"},{"time",timeFunc,"Time: will give you the time of a timezone"},{"prod",prod,"Prod: will give you the product and the average of the numbers"},{"files",files,"files: Will display all the files in the directory"},{"convert",convert,"Convert: use double quotes to put in your conversion ex:  \"feet\" to \"meters\" "}, {NULL,NULL,NULL}} ;   
   	
char *cmd[10]={"help","time","prod","files", "convert"} ; 
char *(*methods[10])()={help,timeFunc,prod,files,convert}; //function pointer with brackets. Can take anything  HERE
char *helpMsg[100]={"Help: This commands lists all the commands.","Time: will give you the time of a timezone","Prod: will give you the product and the average of the numbers","Files: Will display all the files in the directory","Convert: use double quotes to put in your conversion ex:  \"feet\" to \"meters\" "};   


//When SIGHUP is sent to this program, the functions stored in methods and the

//commands stored in commands will load a new set of commands and routines.
void loadMethods(int signum) {
    int i=0; 
    char command[30],libraryName[100], methodName[30], helpmsg[100];
    char inputLine[500]; 
	
	

    fprintf(stdout,"SIGHUP received.  Modifying commands recognized\n"); 
    FILE *fp=fopen("config","r"); 
    if(fp) {
            bzero(cmd, sizeof cmd);
            bzero(methods,sizeof methods); 
            while(fgets(inputLine,sizeof inputLine,fp))  
            {    

               if(inputLine[0]!='#') 
               {
                   sscanf(inputLine,"%[^,],%[^,],%[^,],%[^\n]",command,libraryName,methodName,helpmsg); 
                   fprintf(stdout,"%s  %s   %s  %s\n", command ,libraryName,methodName,helpmsg);  
                   void *libHandle=dlopen(libraryName,RTLD_LAZY);
                       if(!libHandle) { fprintf(stderr,"%s\n",dlerror()); exit(1); } 
                       cmdStruct[i].cmd=strdup(command); 
                       cmdStruct[i].method=dlsym(libHandle,methodName);
			  cmdStruct[i].helpMeth=dlsym(libHandle,helpmsg);

                       if(!cmdStruct[i].method) { fprintf(stderr,"%s\n",dlerror()); exit(1);}  
                   i++;  
               }   
              else fprintf(stdout,"Comment: %s\n",inputLine); 
             }

          } 
    else perror("config"); 
    fclose(fp); 
    fprintf(stdout,"Finished reconfiguring commands\n"); 

  }  


char *interpret(char *cmdline)
{
  char **tokens;
  char *cmd,*cmd2;
 
 // char *result; 

  tokens=history_tokenize(cmdline);   //Split cmdline into individual words.  
  if(!tokens) return "no response needed"; 
	cmd=tokens[0];   
		
	for(int i=0;cmdStruct[i].cmd;i++)
		if(strcasecmp(cmd,cmdStruct[i].cmd)==0) 
			return (cmdStruct[i].method)(cmd,&tokens[1]);    
 
	return "command not found"; 		
}



int main(int argc, char * argv[],char * envp[])
{ 
 


	history_comment_char='#';
	char incomingMsg[100];
	char *response; 
	char *cmdLine;
  	char cmd[100]; 
       int  nBytes; //size of msg rec'd

  	signal(SIGINT,ctrlCHandler); 
 	 signal(SIGPIPE,psignalHandler); 
	signal(SIGHUP,loadMethods); 

 fprintf(stdout,"Hi, I'm process %d\n",getpid() );  


unlink("SyedPipeIn"); //Destroy any previous version  
  unlink("SyedPipeOut"); //Destroy any previous version  
  if(mkfifo("SyedPipeIn",0600 ))  { perror("serverPipeIn");  exit(1); }
  if(mkfifo("SyedPipeOut",0600 )) { perror("serverPipeOut"); exit(1); }
 

  fprintf(stdout,"The commands for this program are: help , time , prod, files, convert\n"); 
int SyedPipeIn=open("SyedPipeIn",O_RDONLY);
  int SyedPipeOut=open("SyedPipeOut",O_WRONLY);

  while(true) {
	memset(incomingMsg,0,sizeof incomingMsg);
nBytes=read(SyedPipeIn,incomingMsg,sizeof incomingMsg);
if(!nBytes) { fprintf(stdout,"Client has closed the connection\n");break; } 
fprintf(stdout,"Rec'd message: %*s # msg size:  %d\n",nBytes-1,incomingMsg,nBytes-1);  

         cmdLine= incomingMsg; 
	char *response=interpret(cmdLine); 
		
	//fprintf(stdout,"%s\n",response);           

	if(!response) break; 
           
              
         //Send a response to the client
          
          write(SyedPipeOut,response,strlen(response));
        }  

  fprintf(stdout,"Server is now terminated \n"); 
  return 0;

  



  }
