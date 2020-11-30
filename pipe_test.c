#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1
int main(void)
{
  char write_msg[BUFFER_SIZE] = "STRING A";
  char write_msg2[BUFFER_SIZE] = "STRING B";

  char read_msg[BUFFER_SIZE];
  char read_msg2[BUFFER_SIZE];

  int fd[2];
  pid_t pid;

  /* create the pipe */
  if (pipe(fd) == -1)
  {
    fprintf(stderr, "Pipe failed");
    return 1;
  }
  /* fork a child process */
  pid = fork();
  if (pid < 0)
  { /* error occurred */
    fprintf(stderr, "Fork Failed");
    return 1;
  }
  if (pid > 0)
  { /* parent process */
   
    /* write to the pipe */
    write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);

    // waiting for all of the child processes
    int stat;
    pid_t finished_child_id;
    while ((finished_child_id = wait(&stat)) != -1)
    {
      printf("child process is finished with id %d\n", finished_child_id);
    }

    read(fd[READ_END], read_msg2, BUFFER_SIZE);
    printf("read from parent: %s\n", read_msg2);
  }
  else
  { /* child process */

    /* read from the pipe */
    read(fd[READ_END], read_msg, BUFFER_SIZE);
    printf("read from child: %s\n", read_msg);

    sleep(4); // sleep the child process

    write(fd[WRITE_END], write_msg2, strlen(write_msg2) + 1);

    return 0;
  }

  
}