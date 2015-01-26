/*
A SIMPLE SHELL IN C

UG201310005
AMIT JAIN

UG201310001
AAKASH HASIJA


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>

struct command_t {
        char *name;
        int argc;
        char *argv[64];
};

DIR *dp;
struct dirent *printls;
mode_t y=0x777;
int count,i;
struct direct **files;


static char commandInput = '\0';
static int buf_chars = 0;
static char *pathv[64];
static char commandLine[64];
static char dir[64];

void print_dir();
void command_with_output(int i);
void command_with_input(int i);
void command_with_pipe(int i);
void clearScreen();
int checkValid();
int parseCommand(char * commandLine, struct command_t * command) ;
int buffer_read(char * buffer, char * commandInput);

struct command_t command;
struct dirent *printls;

// **************************************** MAIN FUNCTION*******************************************

int main(){

while(1) {

	print_dir();


	commandInput = getchar();
	if(commandInput == '\n') {
		continue;
	}
	else {
		buffer_read(commandLine, &commandInput);

		if((strcmp(commandLine, "exit") == 0))
			break;

		parseCommand(commandLine, &command);

		if(checkValid() == 0)
		{
		printf("error:command not found\n");
		continue;
		}
	}
}

printf("\n");

}


		// print the prompt
void print_dir() {
        char dir1[100];
	getcwd(dir1,sizeof(dir1));
	printf("ashell/%s ->",dir1);
}

		/* OUTPUT OPERATOR */
void command_with_output(int i){
	FILE *fp;

	command.argv[i]=NULL;

 	pid_t pid=fork();
 	if(pid==-1){
   		printf("child not created\n");
  	}
 	if(pid==0){
 
       		 int file = open(command.argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
       		dup2(file,1);
    		if(checkValid()==0)
    		{
      			printf("error:command not found\n");     
    		}
    		close(file);
     		kill(getpid(),SIGKILL);
 	}
 	else
	{
		int status;
		do
		{
			waitpid(pid,&status,WUNTRACED | WCONTINUED);
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

		/* OUTPUT APPEND OPERATOR */
void command_with_outputappend(int i){
	FILE *fp;

 	command.argv[i]=NULL;

 	pid_t pid=fork();
	if(pid==-1){
   		printf("child not created\n");
  	}
 	if(pid==0){
 
    	    int file = open("xypqzr.txt",O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
    	   dup2(file,1);
    		if(checkValid()==0)
    		{
      		printf("error:command not found\n");     
    		}
   		 close(file);
    	FILE * source = fopen("xypqzr.txt","r+");
    	FILE * dest = fopen(command.argv[i+1],"a+");
    	char x;
   	
	 while(!feof(source)){
	      fscanf(source,"%c",&x);
	      fprintf(dest,"%c",x);
	    }
    	fclose(source);
    	fclose(dest);
    	 kill(getpid(),SIGKILL);
 	}
 	else
	{
		int status;
		do
		{
		waitpid(pid,&status,WUNTRACED | WCONTINUED);
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

		/* INPUT OPERATOR */
void command_with_input(int i){

	FILE *fp;

	 command.argv[i]=NULL;
 
	 pid_t pid=fork();
	 if(pid==-1){
	   printf("child not created\n");
	  }
	 if(pid==0){
 
 	   int file = open(command.argv[i+1], O_RDONLY, S_IRWXU | S_IRGRP | S_IROTH);
 	      dup2(file,0);
 	   if(checkValid()==0)
 	   {
 	     printf("error:command not found\n");     
 	   }
 	   close(file);
 	    kill(getpid(),SIGKILL);
 	}
 	else
	{
		int status;
		do
		{
		waitpid(pid,&status,WUNTRACED | WCONTINUED);
		}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

}


			/* PIPE OPERATOR */
void command_with_pipe(int i){
 	pid_t pid;
	command.argv[i]=NULL;
 	int fd[2];
    	pid_t childpid;
    	pipe(fd);
    	childpid=fork();
    	if (childpid == -1)
    	{
        	perror("Error forking...");
        	exit(1);
    	}
 	if (childpid==0)  
    	{
        	close(fd[0]);   
                dup2(fd[1],1);
        	 if(checkValid()==0)
    		{
      			printf("error:command not found\n");     
    		}
   		kill(getpid(),SIGKILL);
    	}
    	if (childpid)   /*parent proces*/  
    	{  
       		pid_t pid3 = fork();
      		if(pid3==0)
		{
	        	wait(&childpid);        //waits till the child send output to pipe
	        	close(fd[1]);
	               dup2(fd[0],0);
	       		 execlp(command.argv[i+1],command.argv[i+1],command.argv[i+2],NULL);
			kill(getpid(),SIGKILL);
	       }
     
	} 
}



// check the command is there or not
int checkValid() {

    i=1;
			/* CHECK FOR THE OPERATOR */
    while(command.argv[i]!=NULL){
        if(strcmp(command.argv[i],">>")==0)
        {
        command_with_outputappend(i);
        return 1;
        }
        else if(command.argv[i][0]=='<')
        {
        command_with_input(i);
   	 return 1;
        }
        else if(command.argv[i][0]=='|')
        {
        command_with_pipe(i);
   	 return 1;
        }
    	else if(command.argv[i][0]=='>')
   	 {
         command_with_output(i);
     	return 1;
    	}
    	 i++;
    }

		/* CHECK FOR THE COMMANDS*/
	if(strcmp("chdir", command.argv[0]) == 0||strcmp("cd", command.argv[0]) == 0) { 		// CHDIR CHECKING
		if (command.argv[1] == NULL) {
		chdir(getenv("HOME"));
		}
		else {
			getcwd(dir,sizeof(dir));
			strcat(dir,"/");
			strcat(dir,command.argv[1]);
			if (chdir(dir) == -1) {
 				 printf("%s: no such directory\n", command.argv[1]);
			}
		}
	return 1;
	}

	else if(strcmp("clear", command.argv[0]) == 0) { 		// CLEAR CHECKING
		clearScreen();
		return 1;
	}
	else if(strcmp(command.argv[0],"pwd")==0){ 		// PWD 
                char dir1[100];
		getcwd(dir1,sizeof(dir1));
       		printf("%s\n",dir1);
		return 1;
     	}
	else if(strcmp(command.argv[0],"ls")==0){ 		//LS
		char dir2[100];
		getcwd(dir2,sizeof(dir2));
                 dp=opendir(dir2);
 		printls=readdir(dp);

		while(printls!=NULL)
   		{
        		 puts(printls->d_name);
    			printls=readdir(dp);
   		}
		printf("\n");
		closedir(dp);
 
		return 1;
     	}
	else if(strcmp(command.argv[0],"mkdir")==0){ 		 // MKDIR
		mode_t process_mask = umask(0);
	       mkdir(command.argv[1], S_IRWXU | S_IRWXG | S_IRWXO);
		umask(process_mask);
	        printf("shell-> directory %s created\n",command.argv[1]);
		return 1;
	}
	else if(strcmp(command.argv[0],"rmdir")==0){		//RMDIR
		rmdir(command.argv[1]);
		printf("shell-> directory %s removed\n",command.argv[1]);
		return 1;
	}
	else if(command.argv[0][0]=='.' || command.argv[0][0]=='/'){ 		//EXECUTING FILES USING PATH
		pid_t pid;
		char *a[64]={NULL},*env[64]={NULL};

		pid=fork();
		if(pid==-1){
			printf("child was not created");
		}
		else if(pid==0)
		{
			execve(command.argv[0],a,env);
			perror("execve");   /* execve() only returns on error */
		           exit(EXIT_FAILURE);
		}
		else
		{
			if(command.argv[1]!=NULL)
			{
				if(command.argv[1][0]=='&')
				{
					printf("Process reated with PID: %d\n",pid);
				}
			}
			else
			{
				waitpid(pid,NULL,0);
			}

		}
		return 1;
	}

	else if(strcmp(command.argv[0],"gedit")==0) 		//GEDIT 
	    {
		     pid_t pid1;
			 pid1=fork();
		     if(pid1==-1){
			printf("child was not created");
			}
	       	else if(pid1==0)
			{
			execlp(command.argv[0],command.argv[0],command.argv[1],NULL);
			perror("execlp");   /* execve() only returns on error */
		           exit(EXIT_FAILURE);
			}
		else
			{
			if(command.argv[2]!=NULL)
			{
				if(command.argv[2][0]=='&')
				{
					printf("Process created with PID: %d\n",pid1);
				}
			}
			else
			{

			printf("i am waiting for PID: %d\n",pid1);
			waitpid(pid1,NULL,0);

			}
		}
		return 1;
	    }

	else if(strcmp(command.argv[0],"grep")==0){ 		// GREP
	   	pid_t pid1;
		 pid1=fork();
	     	if(pid1==-1){
	   		printf("child was not created");
	    	}
	  	else if(pid1==0){
	    		puts(command.argv[0]);
			puts(command.argv[1]);
	   		  char * argument[]={command.argv[0],command.argv[1],command.argv[2],NULL};
	      		execvp("grep",argument); 
	    
	    		kill(getpid(),SIGKILL);
	  	}
	 	else
		 waitpid(pid1,NULL,0);
		 return 1;
	 }

	else if(strcmp(command.argv[0],"cat")==0){		// CAT
	   	 FILE *src = fopen(command.argv[1],"r+");
	   	 char x;
	   	 while(!(feof(src))){
	   		  fscanf(src,"%c",&x);
	   		  printf("%c",x);
	   	}
	  	fclose(src);
	  	return 1;
	 }

	return 0;
}


// this function parses commandLine into command.argv and command.argc
int parseCommand(char * commandLine, struct command_t * command) {
	char * pch;
	pch = strtok (commandLine," ");
	int i=0;
	while (pch != NULL) {
		command->argv[i] = pch;
		pch = strtok (NULL, " ");
		i++;
	}
	command->argc = i;
	command->argv[i++] = NULL;

	return 0;
}

// this function read user input and save to commandLine
int buffer_read(char * buffer, char * commandInput) {

	buf_chars = 0;

	while((*commandInput != '\n') && (buf_chars <64)) {
		buffer[buf_chars++] = *commandInput;
		*commandInput = getchar();
	}
	buffer[buf_chars] = '\0';

	return 0;
}

// clear the screen
void clearScreen() {
	printf("\033[2J\033[1;1H");
}
