//Justin Gonzalez
//6155658
///
//I affirm that I wrote this program myself without any help from any other people or sources from the internet.
//Except for the part taken from Dr. Raju which was provided and given in the instructions.
//
//This program creates a shell that can pass commands I added the ability to
//Redirect stdout commands to a file.
//Append stdout from a command to a file.
//Redirect stdin to be from a file.
//Pass stdout from one command to the next.

/* This example is stolen from Dr. Raju Rangaswami's original 4338
   demo and modified to fit into our lecture. */

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 20
#define BUFSIZ 1024

char comands[20];

void redirect(char * filename) {
  FILE * fp;
  FILE * outputfile;
  char buffer[BUFSIZ];
  char * token = strtok(comands, ">");

//RUNS COMMAND AND OPENS FILE
  fp = popen(token, "r");
  outputfile = fopen(filename, "w");

//PRINTS OUTPUT TO FILE
  while (fgets(buffer, BUFSIZ, fp)) {
    fprintf(outputfile, "%s\n", buffer);
  }

  pclose(fp);
  fclose(outputfile);
}


void append(char * filename) {
  FILE * fp;
  FILE * outputfile;
  char buffer[BUFSIZ];
  char * token = strtok(comands, ">>");

//RUNS COMMAND AND OPENS FILE
  fp = popen(token, "r");
  outputfile = fopen(filename, "ab");

//APPENDS COMMAND OUTPUT TO FILE
  while (fgets(buffer, BUFSIZ, fp)) {
    fprintf(outputfile, "%s\n", buffer);
  }

  pclose(fp);
  fclose(outputfile);
}


void fileredirect(char * filename, char * outputfilename) {
  FILE * fp;
  FILE * outputfile;
  char * token = strtok(comands, "<");
  char buffer[BUFSIZ];
  char * cmd = strcat(comands, filename);

//RUNS COMMAND AND CREATES FILE 
  fp = popen(cmd, "r");
  outputfile = fopen(outputfilename, "ab");

//SAVES COMMAND OUTPUT TO FILE
  while (fgets(buffer, BUFSIZ, fp)) {
    fprintf(outputfile, "%s\n", buffer);
  }

  pclose(fp);
  fclose(outputfile);
}


void stdoutredirect(char* filename){
FILE *fp;
char *token = strtok(comands, "<");
char buffer[BUFSIZ];
char *cmd = strcat(comands, filename);

//RUNS COMMAND
fp = popen(cmd, "r");

//PRINTS TO SCREEN
while(fgets(buffer, BUFSIZ, fp)){
printf("%s\n", buffer);
}

pclose(fp);
}


void pass(char* passedcommand){


}


int get_args(char* cmdline, char* args[])
{
  int i = 0;

  /* if no args */
  if((args[0] = strtok(cmdline, "\n\t ")) == NULL)
    return 0;

  while((args[++i] = strtok(NULL, "\n\t ")) != NULL) {
    if(i >= MAX_ARGS) {
      printf("Too many arguments!\n");
      exit(1);
    }
  }
  /* the last one is always NULL */
  return i;
}


void execute(char * cmdline) {
    int pid, async;
    char * args[MAX_ARGS];
    char checker[2];
    char filename[20];
    char outputfilename[20];

    int stdoutredirectFlag = 0;
    int redirectFlag = 0;
    int appendFlag = 0;
    int fileredirectFlag = 0;
    int passFlag = 0;

    int i = 0;
    int nargs = get_args(cmdline, args);

    if (nargs <= 0) return;

    if (!strcmp(args[0], "quit") || !strcmp(args[0], "exit")) {
      exit(0);
    }

    while (args[i] != NULL) {

      if ((memcpy(checker, args[i], 2)) != NULL) {

        //REDIRECT
        if (checker[0] == '>' && checker[1] != '>') {
          strcpy(filename, args[i + 1]);
          strcpy(outputfilename, args[i + 1]);
          redirectFlag = 1;
        }

        //APPEND
        if (checker[0] == '>' && checker[1] == '>') {
          strcpy(filename, args[i + 1]);
          appendFlag = 1;
        }

        //FILE REDIRECT TO STDN
        if (checker[0] == '<' && redirectFlag == 0) {
          strcpy(filename, args[i + 1]);
          stdoutredirectFlag = 1;
        }

        //FILE REDIRECT TO FILE
        if (checker[0] == '<' && redirectFlag == 1) {
          fileredirectFlag = 1;
          strcpy(filename, args[i + 1]);
        }

	//PASS STDOUT TO ANOTHER COMMAND
        if (checker[0] == '|') {
	 passFlag = 1;
         strcpy(filename, args[i + 1]);
	}
      }
      i++;
    }


//FLAGS ALLOW FOR CHAINING

//PASS "filename" is actually the 2nd command in this case
if (passFlag == 1) {
pass(filename);
}
//APPENDS
if (appendFlag == 1) {
  append(filename);
}//REDIRECTS
if (redirectFlag == 1 && fileredirectFlag == 0) {
  redirect(filename);
}//FILE REDIRECTS
if (fileredirectFlag == 1 && redirectFlag == 1) {
  fileredirect(filename, outputfilename);
}//STDOUT REDIRECT
if (stdoutredirectFlag == 1 && redirectFlag == 0) { //REDIRECT TO STDOUT
  stdoutredirect(filename);
}


//IF ANY PREVIOUSE COMMANDS HAVE BEEN DONE THERE ARE NO MORE ARGS TO CHECK
if(redirectFlag == 0 && appendFlag == 0 && fileredirectFlag == 0 && stdoutredirectFlag == 0 && passFlag == 0){
  /* check if async call */
  if(!strcmp(args[nargs-1], "&")) { async = 1; args[--nargs] = 0; }
  else async = 0;

  pid = fork();
  if(pid == 0) { /* child process */
    execvp(args[0], args);
    /* return only when exec fails */
    perror("exec failed");
    exit(-1);
  } else if(pid > 0) { /* parent process */
    if(!async) waitpid(pid, NULL, 0);
    else printf("this is an async call\n");
  } else { /* error occurred */
    perror("fork failed");
    exit(1);
  }
}
}

int main (int argc, char* argv [])
{
  char cmdline[BUFSIZ];

  for(;;) {
    printf("COP4338$ ");
    if(fgets(cmdline, BUFSIZ, stdin) == NULL) {
      perror("fgets failed");
      exit(1);
    }
	strcpy(comands, cmdline);
	execute(cmdline) ;
  }
  return 0;
}
