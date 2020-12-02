#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1

struct Process {
  pid_t id;
  int pipe[2];
  int process_level;
};

int get_process_exe_time(char* command);
void extract_file_data(char commands[][BUFFER_SIZE], FILE *commands_file);
void run_first_level_process(int p_pipe[2], int n_scnd_level_process);
void run_second_level_process(int p_pipe[2]);
void splitString(char*, char[][BUFFER_SIZE]);

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

  extract_file_data(commands, commands_file);

  struct Process fst_level_processes[n_fst_level_process];

  // create first level child processes
  for (int i = 0; i < n_fst_level_process; i++){

    // create new pipe for parent and new level 1 process
    if (pipe(fst_level_processes[i].pipe) == -1){
      printf("Pipe failed");
      return 1;
    }
    
    pid_t new_id = fork(); // create new child process

    if (new_id > 0){ // parent process
      
      fst_level_processes[i].id = new_id;
      fst_level_processes[i].process_level = 1; // set the process level to 1
      
      // compact all the commands for i-th 1st level child
      char command[BUFFER_SIZE] = "";
      for (int j = 0; j < n_scnd_level_process; j++){
        strcat(command, commands[i * n_scnd_level_process + j]);
        strcat(command, "$");   // seperate commands with unique character
      }

      // send batch of commands to 1st level child process
      write(fst_level_processes[i].pipe[WRITE_END], command, BUFFER_SIZE);  

    }else if (new_id == 0){ // first level child process
      run_first_level_process(fst_level_processes[i].pipe, n_scnd_level_process);
      break;  // stop continuing the loop in child process
    }

  }
  // waiting for all of the child processes
  int stat;
  pid_t finished_child_id;
  while ((finished_child_id = wait(&stat)) != -1){
    printf("child process is finished with id %d\n", finished_child_id);
  }

  fclose(commands_file);
  return 0;
}

// whatever the level 1 process need to do
void run_first_level_process(int p_pipe[2], int n_scnd_level_process){
  char command[BUFFER_SIZE];
  read(p_pipe[READ_END], command, BUFFER_SIZE);  
  //printf("received command Child process with id %d: %s\n", getpid(), command);


  char buffer[n_scnd_level_process][BUFFER_SIZE]; // for saving commands of the n-th child
  
  // extract commands from string recieved from pipe
  splitString(command, buffer);

  // store second level processes
  struct Process scnd_level_processes[n_scnd_level_process];

  // create 2nd level child processes
  for (int i = 0; i < n_scnd_level_process; i++){
    // create pipe
    if (pipe(scnd_level_processes[i].pipe) == -1){
      printf("Pipe failed");
      return;
    }

    pid_t new_id = fork(); // create new child process
    if (new_id > 0){ // parent process
      scnd_level_processes[i].id = new_id;
      scnd_level_processes[i].process_level = 2;

      // write each command to a child process pipe
      write(scnd_level_processes[i].pipe[WRITE_END], buffer[i], strlen(buffer[i]));

    }else if (new_id == 0){// child process
      run_second_level_process(scnd_level_processes[i].pipe);
      break; // stop looping through
    }

  }

  // wait for all the 2nd level processes
  pid_t finished_child_id;
  int stat;
  while ((finished_child_id = wait(&stat)) != -1){
    printf("2nd level process finished with id %d\n", finished_child_id);
  }

  return;
}

// split string with the special character $ and store each substring to buffer
void splitString(char * string, char buffer[][BUFFER_SIZE]){
  int child_num = 0; // keep track of the child we want to set the command for
  int k = 0;

  for (int i = 0; i < strlen(string); i++){
    if (string[i] =='$'){
      buffer[child_num][k] = 0;
      child_num++;
      k = 0;
      continue;
    }
    buffer[child_num][k] = string[i];
    k++;
  } 
}


// run the code for second level process
void run_second_level_process(int p_pipe[2]){
  printf("@@2nd level process executing\n");
  return;
}

// read from file and extract all the commands and their exe time
void extract_file_data(char commands[][BUFFER_SIZE], FILE *commands_file){
  char buffer[BUFFER_SIZE];
  // reading file line by line
  int counter = 0;
  while (fgets(buffer, BUFFER_SIZE, commands_file) != NULL){
        
    int len = strlen(buffer);
    if (len == 1) continue; // avoid new line character
    buffer[len-1] = 0;

    // store the results
    strcpy(commands[counter], buffer);
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
