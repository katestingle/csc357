#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child: read from pipe
        close(fd[1]); // close write end (important)

        char buf[256];
        ssize_t n = read(fd[0], buf, sizeof(buf) - 1);
        if (n < 0) {
            perror("read");
            exit(1);
        }
        buf[n] = '\0';

        printf("Child read: %s\n", buf);

        close(fd[0]);
        exit(0);
    } else {
        // Parent: write to pipe
        close(fd[0]); // close read end (important)

        const char *msg = "Hello from parent via pipe!";
        ssize_t n = write(fd[1], msg, strlen(msg));
        if (n < 0) {
            perror("write");
            return 1;
        }

        close(fd[1]); // closing signals EOF to child
        wait(NULL);
    }

    return 0;
}