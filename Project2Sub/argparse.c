#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>

#define FALSE (0)
#define TRUE  (1)

/*
* argCount is a helper function that takes in a String and returns the number of "words" in the string assuming that whitespace is the only possible delimeter.
*/
static void argCount(char*line,int* argcp)
{
  bool inWord = false;
  *argcp = 0;
  for (int i = 0; i < strlen(line) ; i++){
    if ((line[i] != ' ' && line[i] != '\n') && !inWord){
      (*argcp)++;
      inWord = true;
    } else if(line[i] == ' '){
      inWord = false;
    }
  }
}



/*
*
* Argparse takes in a String and returns an array of strings from the input.
* The arguments in the String are broken up by whitespaces in the string.
* The count of how many arguments there are is saved in the argcp pointer
*/
char** argparse(char* line, int* argcp)
{ 
  int i = 0;
  int j = 0;
  int k = 0;
  char* beggining_word;
  //updates argcount to argcp
  argCount(line,argcp);
  //check return if 0 arguments
  if(*argcp == 0){
    return (void*)0;
  }
  char** result = calloc(*argcp, sizeof(char*));
  char** result_address = result;
 
  bool inWord = false;
//iterate to the end of line
  while(line[i] != '\0'){
    if(line[i] == ' ' | line[i] == '\n'){
      //checks if we reach a new line or space immediately preceeding a char
      if(inWord){
        line[i] = '\0';
        result[j] = malloc(k+1);
        inWord = false;
        memcpy(result[j],beggining_word, k+1);
        j++;
        k = 0;
      }
    } else{
      //if we reach the first character of an argument
      if(!inWord){
        beggining_word = &line[i];
        inWord = true;
      }
      k++;
    }
    i++;
  }
  return result_address;
}

