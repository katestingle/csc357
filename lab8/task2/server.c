#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 1024


int check_child_processes(int *active_processes) {
   int status; 
   pid_t finished_pid = waitpid(-1, &status, WNOHANG); // 0 if no child processes have finished, otherwise returns the PID of a finished child process
   if (finished_pid > 0) { // A child process has finished
      *active_processes = *active_processes - 1; // Decrement active process count when a child process finishes
      if(WIFSIGNALED(status)) { // child exited w/ signal
         fprintf(stdout, "process %d completed\n", finished_pid);
         if(WTERMSIG(status) == SIGCHLD) { // child exited w/ SIGCHLD signal
            fprintf(stderr, "Child process was killed by signal %d.\n", SIGCHLD);
         } else {
            fprintf(stderr, "Child process was killed by signal %d.\n", WTERMSIG(status));
         }
      }
      return 0; // Successfully handled a finished child process
   } else if (finished_pid == -1) { // An error occurred while waiting for child processes
      fprintf(stderr, "Error waiting for child process %d, exiting with error", finished_pid);
      return 1;
   }
   return 0; // No child processes have finished, but no error occurred
}

void sig_handler(int signum) {
   fprintf(stderr, "Signal %d received.\n", signum);
   int status;
   pid_t finished_pid = waitpid(-1, &status, WNOHANG); // check for the child process, and if it has finished, update the active_processes count accordingly
   while(finished_pid >0) { // Loop to handle multiple finished child processes if necessary
      if(WIFSIGNALED(status)) { // child exited w/ signal
         fprintf(stdout, "process %d completed\n", finished_pid);
         if(WTERMSIG(status) == SIGCHLD) { // child exited w/ SIGCHLD signal
            fprintf(stderr, "Child process %d was killed by signal %d.\n", finished_pid, SIGCHLD);
         } else {
            fprintf(stderr, "Child process %d was killed by signal %d.\n", finished_pid, WTERMSIG(status));
         }
      }
      finished_pid = waitpid(-1, &status, WNOHANG); // Check for any additional finished child processes
   }
}


void handle_request(int nfd)
{
   FILE *network = fdopen(nfd, "r");
   char *line = NULL;
   size_t size;
   ssize_t num;

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }

   while ((num = getline(&line, &size, network)) >= 0)
   {
      // echo the request back to the client
      write(nfd, line, num);
   }

   free(line);
   fclose(network);
}

void run_service(int fd)
{
   int active_processes = 0;
   while (1)
   {
      int nfd = accept_connection(fd);
      if (nfd != -1)
      {
         pid_t pid = fork();
         if (pid == -1){
            perror("fork");
            close(nfd);
            continue;
         }
         else if (pid > 0) {
            // parent process registers a signal handler for the SIGCHLD signal (raised when a child process completes) and wait within that handler
            active_processes++; // Increment active process count when a new child process is created
            fprintf(stdout, "fork success: %d, num active processes: %d\n", pid, active_processes);
            struct sigaction sa = {0};
            sa.sa_handler = sig_handler;
            sa.sa_flags = WNOHANG; // Set the WNOHANG flag to prevent blocking if no child processes have finished
            // sigemptyset(&sa.sa_mask); 

            // Register the sigaction for SIGCHLD
            if (sigaction(SIGCHLD, &sa, NULL) == -1) { // WNOHANG option to prevent blocking if no child processes have finished
                  perror("sigaction stringency failed");
                  exit(1);
            }

            if (active_processes > 0) {
               if(check_child_processes(&active_processes) != 0) { // calls wait() to check for finished child processes and update active_processes count
                  return; // exit if there was an error waiting for child processes
               }
            }
         } else {
            // child process
            printf("Connection established\n");
            handle_request(nfd);
            printf("Connection closed on process %d\n", getpid());
            return;
         }
      }
   }
}

int main(void)
{
   int fd = create_service(PORT);

   if (fd == -1)
   {
      perror(0);
      exit(1);
   }

   printf("listening on port: %d\n", PORT);
   run_service(fd);
   close(fd);
   return 0;
}
