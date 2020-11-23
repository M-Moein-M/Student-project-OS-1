#include <string.h>
#include <stdio.h>

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
  while (fgets(buffer, 100, commands_file) != NULL){
    printf("*%s*", buffer);
  }

  fclose(commands_file);
  return 0;
}