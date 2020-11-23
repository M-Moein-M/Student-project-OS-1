#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int get_process_exe_time(char* command);

int main()
{

  // loading and reading commands file
  FILE *commands_file = fopen("commands.txt", "r");

  if (commands_file == NULL){
    printf("Commands file was not found\n");
    return 1;
  }

  // extracting number of commands and each level processes
  int n_fst_level_process, n_scnd_level_process;
  fscanf(commands_file, "%d", &n_fst_level_process);
  fscanf(commands_file, "%d", &n_scnd_level_process);

  // data structures for number of processes and commands and their execution time
  int n_commands = n_fst_level_process * n_scnd_level_process;
  
  char buffer[100];
  char commands[n_commands][100];
  int commands_execution_time[n_commands];

  // reading file line by line
  int counter = 0;
  while (fgets(buffer, 100, commands_file) != NULL){
        
    int len = strlen(buffer);
    buffer[len-1] = 0;

    int exe_time = get_process_exe_time(buffer);
    if (exe_time == -1) continue;
      
    printf("%s  => ", buffer);
    printf("%d\n", exe_time);

    // store the results
    strcpy(commands[counter], buffer);
    commands_execution_time[counter] = exe_time;
    
  }

  fclose(commands_file);
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
