#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1

int get_process_exe_time(char* command);


void main(int argc, char* argv[]){
  char *command = argv[1];
  int exe_time = get_process_exe_time(command);
  int pip_write_end = atoi(argv[2]);

  sleep(exe_time); // sleep for the given amount in input
  printf("command: %-25s  exe-time: %-5d  pipe_write: %-10d\n", command, exe_time,pip_write_end);
  
  return;
}

// returns last written number in each line of commands file and removes it from input string
int get_process_exe_time(char* command){
  int len = strlen(command);

  if (len == 0) return -1;

  int i = len;
  char c = command[i];
  while (c != ' '){
    i--;
    c = command[i];
  }
  
  int temp = i;

  char exe_time_str [10];
  for (int j = 0; i<len; i++){
    exe_time_str[j] = command[i];
    j++;
  }

  command[temp] = 0;

  return atoi(exe_time_str);
}
