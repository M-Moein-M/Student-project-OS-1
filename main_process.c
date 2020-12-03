#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1

struct Process {
  pid_t id;
  int pipe[2];
  int process_level;
};

void intToString(int num, char str[]);
void extract_file_data(char commands[][BUFFER_SIZE], FILE *commands_file);
void run_first_level_process(int p_pipe[2],int pipe_pool[][2],int n_fst_level_process, int n_scnd_level_process, int process_number);
void run_second_level_process(int p_pipe[2]);
void splitString(char*, char[][BUFFER_SIZE]);
int* get_pipe_from_pool(int process_level, int process_number, int pipe_pool[][2], int n_fst_level_process);

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

  // creating pipe pool
  int all_needed_pipes = n_fst_level_process + n_fst_level_process * n_scnd_level_process;
  int pipe_pool[all_needed_pipes][2];
  for (int i = 0; i < all_needed_pipes; i++){
    // create new pipe
    if (pipe(pipe_pool[i]) == -1){
      printf("Pipe failed");
      return 1;
    }
  }

  struct Process fst_level_processes[n_fst_level_process];

  // create first level child processes
  for (int i = 0; i < n_fst_level_process; i++){

    // create new pipe for parent and new level 1 process
    int* pipe_temp = get_pipe_from_pool(1, i, pipe_pool, n_fst_level_process);

    // copy pipes end
    fst_level_processes[i].pipe[READ_END] = pipe_temp[READ_END];
    fst_level_processes[i].pipe[WRITE_END] = pipe_temp[WRITE_END];

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
      write(fst_level_processes[i].pipe[WRITE_END], command, strlen(command)+1);  

    }else if (new_id == 0){ // first level child process
      run_first_level_process(fst_level_processes[i].pipe, pipe_pool,n_fst_level_process,n_scnd_level_process, i);
      return 0;  // stop continuing the loop in child process
    }

  }

  int stat;
  // waiting for all of the child processes  
  while (wait(&stat) != -1);

  // read results from pipes connected to level 1 processes
  char output[BUFFER_SIZE];
  for (int i = 0; i < n_fst_level_process; i++){
    int* p_pipe = get_pipe_from_pool(1, i, pipe_pool, n_fst_level_process);
    read(p_pipe[READ_END], output, BUFFER_SIZE);
    printf("%s\n", output);
  }

  fclose(commands_file);
  return 0;
}

// retrieve pipe associated with process
int* get_pipe_from_pool(int process_level, int process_number, int pipe_pool[][2], int n_fst_level_process){
  int* p_pipe;
  if (process_level == 1){ // level 1 process
    p_pipe = pipe_pool[process_number];
  }else{  // level 2 process
    p_pipe = pipe_pool[n_fst_level_process+process_number];
  }
  return p_pipe;
}

// whatever the level 1 process need to do
void run_first_level_process(int p_pipe[2],int pipe_pool[][2],int n_fst_level_process, int n_scnd_level_process, int process_number){
  char command[BUFFER_SIZE];
  read(p_pipe[READ_END], command, BUFFER_SIZE);  // read commands passed in by parent

  char buffer[n_scnd_level_process][BUFFER_SIZE]; // for saving commands of the n-th child
  
  // extract commands from string recieved from pipe
  splitString(command, buffer);

  // store second level processes
  struct Process scnd_level_processes[n_scnd_level_process];
  // create 2nd level child processes
  for (int i = 0; i < n_scnd_level_process; i++){
    // get process pipe
    int* pipe_temp = get_pipe_from_pool(2, n_scnd_level_process*process_number+i, pipe_pool, n_fst_level_process);
    scnd_level_processes[i].pipe[READ_END] = pipe_temp[READ_END];
    scnd_level_processes[i].pipe[WRITE_END] = pipe_temp[WRITE_END];

    pid_t new_id = fork(); // create new child process
    if (new_id > 0){ // parent process
      scnd_level_processes[i].id = new_id;
      scnd_level_processes[i].process_level = 2;

      // write each command to a child process pipe
      write(scnd_level_processes[i].pipe[WRITE_END], buffer[i], strlen(buffer[i])+1);

    }else if (new_id == 0){// child process
      run_second_level_process(scnd_level_processes[i].pipe);
      return; // stop looping through
    }
  }
  
  int stat;
  pid_t finished_child_id;
  // wait for all the 2nd level processes
  while ((finished_child_id = wait(&stat)) != -1){
    printf("level 2 process with id: %d is finished\n", finished_child_id);
  }


  char output[BUFFER_SIZE];
  char fst_level_results[BUFFER_SIZE] = "";
  for(int i = 0; i < n_scnd_level_process; i++){
    read(scnd_level_processes[i].pipe[READ_END], output, BUFFER_SIZE);
    strcat(fst_level_results, output);
    strcat(fst_level_results, "$");   // seperate commands with unique character
  }
  // write the results on pipe and send them back to parent(process level 0)
  write(p_pipe[WRITE_END], fst_level_results, BUFFER_SIZE);

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
  char command[BUFFER_SIZE];
  read(p_pipe[READ_END], command, BUFFER_SIZE);  // read command passed in by parent

  // args for the program we'll run on exec
  char pipe_write_end_to_stirng[50];
  intToString(p_pipe[WRITE_END], pipe_write_end_to_stirng);

  char *args[]={"./execute_command.out", command, pipe_write_end_to_stirng,NULL};
  execv(args[0],args);
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
