/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"


/* PROTOTYPES */

void processline (char *line);
ssize_t getinput(char** line, size_t* size);
void freeing();
char* buffer;

/* main
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.
 */

int main () {  
  //free's the buffer when exit is called
  atexit(freeing);
  size_t buffer_size = 0;
  buffer = calloc(1,sizeof(char*));
  

  while(1){
    printf("%% ");
    getinput(&buffer,&buffer_size);
    processline(buffer);
  }

  return EXIT_SUCCESS;
}
//function to be called at "atexit()"
void freeing(){
  free(buffer);
}

/* getinput
* line     A pointer to a char* that points at a buffer of size *size or NULL.
* size     The size of the buffer *line or 0 if *line is NULL.
* returns  The length of the string stored in *line.
*
* This function prompts the user for input, e.g., %myshell%.  If the input fits in the buffer
* pointed to by *line, the input is placed in *line.  However, if there is not
* enough room in *line, *line is freed and a new buffer of adequate space is
* allocated.  The number of bytes allocated is stored in *size. 
* Hint: There is a standard i/o function that can make getinput easier than it sounds.
*/
ssize_t getinput(char* *line, size_t* size) {

  ssize_t len = getline(line,size,stdin);
  if(len < 0){
    perror("error getting input");
    free(*line);
    exit(EXIT_FAILURE);
  }
  return len;
}


/* processlinehar* array[] = {"builtin.c", "argparse.c"};
  stats(array, 2 );
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 * processline only forks when the line is not empty, and the line is not trying to run a built in command
 */
void processline (char *line)
{
  //check if the input is empty with no characters
  char* line_copy = line;
  while(*line_copy != '\0'){
    if(*line_copy != ' '){
      break;
    }
    line_copy++;
  }
    
  pid_t cpid;
  int   status;
  int arg_Count;
  char** arguments = argparse(line, &arg_Count);

  if(arguments == NULL){
    return;
  }
  //if argument is not in builtin, call execvp on command.
  if(!builtIn(arguments, arg_Count)){
    cpid = fork();
    if(cpid < 0){
      perror("could not fork");
      //if its the child, run execvp
    } else if(cpid == 0){
      if(execvp(arguments[0],arguments)<0){
        perror("error on exec");
        for(int i = 0; i < arg_Count; i++){
        free(arguments[i]);
  }
        free(arguments);
        exit(EXIT_FAILURE);
      }
      //wait if its the parent
    } else if(cpid > 0){
      wait(&status);
      
    }
  }
  //free arguments array of pointers and char arrays before finnishing
  for(int i = 0; i < arg_Count; i++){
    free(arguments[i]);
  }
  free(arguments);
}

