#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

int get_process_exe_time(char* command);
void extract_file_data(char commands[][BUFFER_SIZE], int commands_execution_time [], FILE *commands_file);

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
  
  char commands[n_commands][BUFFER_SIZE];
  int commands_execution_time[n_commands];

  extract_file_data(commands, commands_execution_time, commands_file);

  fclose(commands_file);
  return 0;
}

// read from file and extract all the commands and their exe time
void extract_file_data(char commands[][BUFFER_SIZE], int commands_execution_time [], FILE *commands_file){
  char buffer[BUFFER_SIZE];
  // reading file line by line
  int counter = 0;
  while (fgets(buffer, BUFFER_SIZE, commands_file) != NULL){
        
    int len = strlen(buffer);
    buffer[len-1] = 0;

    int exe_time = get_process_exe_time(buffer);
    if (exe_time == -1) continue; // skip incase it was \n at the end of lines

    // store the results
    strcpy(commands[counter], buffer);
    commands_execution_time[counter] = exe_time;
    
    counter++;
    
  }
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
