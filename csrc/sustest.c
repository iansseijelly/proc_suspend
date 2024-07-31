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

int main() {
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

        // Print memory maps from parent process
        printf("[Child]: Child process PID: %d\n", getpid());
        // Print heap end address
        printf("[Child]: Heap end address: %p\n", sbrk(0));
        // Print heap start address with the dummy malloc
        printf("[Child]: Heap start address: %p\n", dummy);
        // Print stack start address
        unsigned long curr_stack;
        asm("movq %%rsp, %0" : "=r"(curr_stack));
        // print in hex
        printf("[Child]: current stack address: %lx\n", curr_stack);
        printf("#----------------------------------------\n");

        _exit(EXIT_SUCCESS); // Exit child process
    } else {
        // Parent process
        sleep(1); // Ensure the child process has time to stop

        printf("#----------------------------------------\n");
        // Print memory maps from parent process
        printf("[Parent]: Parent process PID: %d\n", getpid());
        // Print heap end address
        printf("[Parent]: Heap end address: %p\n", sbrk(0));
        // Print heap start address with the dummy malloc
        printf("[Parent]: Heap start address: %p\n", dummy);
        // Print stack start address
        unsigned long curr_stack;
        asm("movq %%rsp, %0" : "=r"(curr_stack));
        // print in hex
        printf("[Parent]: current stack address: %lx\n", curr_stack);

        // Print memory maps of the child process
        print_memory_maps(pid);

        // Resume the child process
        kill(pid, SIGCONT);

        // Wait for the child process to finish
        wait(NULL);
    }

    return 0;
}
