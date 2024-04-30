#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <ctype.h>
#include "builtin.h"
#include <string.h>
#include <dirent.h>
#include <linux/stat.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <utime.h>


#define MAX_BUF 256

//Prototypes
static void exitProgram(char** args,int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);
static void stats (char** pathname, int argcp);
static void tails (char** pathname, int argcp);
static void touches (char** pathname, int argcp);


/* builtIn
 ** built in checks each built in command the given command, if the given command
 * matches one of the built in commands, that command is called and builtin returns 1.
 *If none of the built in commands match the wanted command, builtin returns 0;
  */
int builtIn(char** args, int argcp)
{
    if(args == NULL){
      return 1;
    } else if(strcmp(args[0],"exit") == 0){
      exitProgram(args,argcp);
      return 1;
    } else if(strcmp(args[0],"pwd") == 0){
      pwd(args,argcp);
      return 1;
    } else if (strcmp(args[0],"cd") == 0){
      cd(args,argcp);
      return 1;
    }else if (strcmp(args[0],"stat") == 0){
      stats(args,argcp);
      return 1;
    }else if (strcmp(args[0],"touch") == 0){
      touches(args,argcp);
      return 1;
    }else if (strcmp(args[0],"tail") == 0){
      tails(args,argcp);
      return 1;
}
return 0;
}

//exits the shell with a certain value, if value is not inputted, exit with 0. 
static void exitProgram(char** args, int argcp)
{
  int exit_value;
  //check if there's no input for value
  if (argcp == 1){
    exit_value = 0;
  } else if(argcp == 2){
    exit_value = atoi(args[1]);
  } else{
    perror("Wrong number of arguments");
  }
  for(int i = 0; i < argcp; i++){
  free(args[i]);
  }
  free(args);  
  //exit with updated value
  exit(exit_value);

}
//prints the current working directory taking in an array of char arrays (args) and number of arguments (argcp)
static void pwd(char** args, int argcp)
{
  //error checking on arguments
  if (argcp != 1){
    perror("Wrong Number of Arguments");
    return;
  }
  char buf[MAX_BUF];
  if(getcwd(buf, MAX_BUF) == NULL){
    perror("couldn't get current directory");
  } else{
  printf("%s \n", buf);
}
}

//change current working working directory, when no paramaters are entered, cd will change to home directory
static void cd(char** args, int argcp){
  //checks if no arguments entered
  if (argcp == 1){
    int directory = chdir(getenv("HOME"));
    if (directory < 0){
      perror("no change in directory");
    }
  } else if(argcp == 2){
    chdir(args[1]);
  } else{
    perror("wrong number of arguments");
  }
  //changes directory to input
}

//prints information about files
static void stats (char** pathname, int argcp) {
  
  if (argcp == 1){
    perror("Wrong Number of Arguments");
  }
  
  for(int i = 1; i < argcp; i++){
    struct stat sb;

    if (lstat(pathname[i], &sb) == -1){
      perror("lstat");
      exit(EXIT_FAILURE);
    }
    printf("%s\n", pathname[i]);
    printf("ID of containing device:  [%x,%x]\n",
    major(sb.st_dev),
    minor(sb.st_dev));printf("test");

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT) {
      case S_IFBLK:  printf("block device\n");            break;
      case S_IFCHR:  printf("character device\n");        break;
      case S_IFDIR:  printf("directory\n");               break;
      case S_IFIFO:  printf("FIFO/pipe\n");               break;
      case S_IFLNK:  printf("symlink\n");                 break;
      case S_IFREG:  printf("regular file\n");            break;
      case S_IFSOCK: printf("socket\n");                  break;

    default:       printf("unknown?\n");                break;
    }

    printf("I-node number:            %ju\n", sb.st_ino);

    printf("Mode:                     %o (octal)\n",
    sb.st_mode);

    printf("Link count:               %ju\n",  sb.st_nlink);
    printf("Ownership:                UID=%u   GID=%u\n", sb.st_uid,  sb.st_gid);

    printf("Preferred I/O block size: %jd bytes\n", sb.st_blksize);
    printf("File size:                %jd bytes\n", sb.st_size);
    printf("Blocks allocated:         %jd\n", sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));
    printf("\n");

  }
}

//prints last 10 lines of a file to standard output, if more than one file is used, each set of output is prefixed with a header showing the file name
static void tails (char** args, int argcp) {
  
  if (argcp < 2){
    perror("Wrong Number of Arguments");
  }

  for (int i = 1; i < argcp; i++){
    FILE* file_open = fopen(args[i],"r");
   
    if(file_open == NULL){
      perror("tail");
      continue;
    } else{
    printf("File Name: %s\n", args[i]);
    char buffer[10][MAX_BUF];
    char line_first[MAX_BUF];
    
    int j = 0;
    while(fgets(line_first,MAX_BUF,file_open) != NULL){
      strcpy(buffer[j%10],line_first);
      j++;
      }
      int start = 0;
      if(j>10){
        start = j-10;
      }
      for(;start<j;start++){
        printf("%s",buffer[start%10]);
      }
    
    printf("\n");
    fclose(file_open);
    }
  }
}
  
//Sets modification and access time of a file, if a file does not exist, a new file shall be created
static void touches (char** args, int argcp){
  if (argcp < 2){
    perror("Wrong Number of Arguments");
    return;
  }
  for(int i = 1; i < argcp; i++){
    printf("PATHNAME : %s \n", args[i]);
    int opened = open(args[i],O_CREAT, 0644);
    if (opened < 0){
      perror("writing new file ");
    }

//update modification time
    struct timespec time[2];
    clock_gettime(CLOCK_REALTIME, &time[0]);
    clock_gettime(CLOCK_REALTIME, &time[1]);

  }
}
