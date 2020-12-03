#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 1000
#define READ_END 0
#define WRITE_END 1

int get_process_exe_time(char* command);
void intToString(int a, char result[]);

int main(int argc, char* argv[]){
  
  char command[BUFFER_SIZE];
  strcpy(command, argv[1]);

  int exe_time = get_process_exe_time(command);
  int pip_write_end = atoi(argv[2]);
    
  // preparing output
  char msg[BUFFER_SIZE];
  intToString(getpid(), msg);
  strcat(command, " ");
  strcat(command, msg);

  usleep(exe_time*1000*100); // sleep for exe-time
  
  write(pip_write_end, command, strlen(command)+1);
  return 0;
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

// str = toString(num)
void intToString(int num, char str[]){
  int i, rem, len = 0, n;

  n = num;
  while (n != 0){
      len++;
      n /= 10;
  }
  for (i = 0; i < len; i++){
      rem = num % 10;
      num = num / 10;
      str[len - (i + 1)] = rem + '0';
  }
  str[len] = '\0';
}