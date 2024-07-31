#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

void print_memory_maps(pid_t pid) {
    printf("#----------------------------------------\n");
    printf("[Parent]: Printing memory maps for Child PID %d\n", pid);
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("[Parent]: Memory maps for PID %d:\n", pid);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // if line contains "heap" or "stack" print it
        if (strstr(line, "heap") || strstr(line, "stack")){
            printf("[Parent]: %s", line);
        }
    }

    fclose(file);
    printf("#----------------------------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // heap init
    // do a dummy malloc to initialize the heap
    int *dummy = (int *)malloc(sizeof(int));
    if (dummy == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        // Pause the child process
        kill(getpid(), SIGSTOP);
        execvp(argv[1], &argv[1]); 
        perror("execvp");

        _exit(EXIT_FAILURE); // Exit child process
    } else {
        // Parent process
        sleep(1); // Ensure the child process has time to stop

        // Print memory maps of the child process
        print_memory_maps(pid);

        // Resume the child process
        kill(pid, SIGCONT);

        // Wait for the child process to finish
        wait(NULL);
    }

    return 0;
}
