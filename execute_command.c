#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1

void main(int argc, char* argv[]){
  //usleep(atoi(argv[argc-1])*1000); // sleep for the given amount in input
  for (int i = 0; i < argc; i++){
    printf("-- %s\n", argv[i]);
  }
  
  return;
}