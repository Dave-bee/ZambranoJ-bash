#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX 256

void cmd_ls() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("ls");
        return;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        printf("%s\n", ent->d_name);
    }

    closedir(dir);
}

void cmd_pwd() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("pwd");
    }
}

void cmd_cd(char *path) {
    if (path == NULL) {
        fprintf(stderr, "cd: falta argumento\n");
        return;
    }

    if (chdir(path) != 0) {
        perror("cd");
    }
}

void ejecutar_externo(char **args) {
    pid_t pid = fork();

    if (pid == 0) {
        execvp(args[0], args);
        perror("comando externo");
        exit(1);
    } 
    else if (pid > 0) {
        wait(NULL);
    }
    else {
        perror("fork");
    }
}

int parsear(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " \n");
    while (args[i] != NULL && i < MAX - 1) {
        i++;
        args[i] = strtok(NULL, " \n");
    }
    return i;
}

int main() {
    char input[MAX];
    char *args[MAX];

    while (1) {
        printf("BASH> ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin))
            break;

        int n = parsear(input, args);
        if (n == 0) continue;

        if (strcmp(args[0], "ls") == 0) {
            cmd_ls();
        } 
        else if (strcmp(args[0], "pwd") == 0) {
            cmd_pwd();
        }
        else if (strcmp(args[0], "cd") == 0) {
            cmd_cd(args[1]);
        }
        else if (strcmp(args[0], "exit") == 0) {
           // printf("saliendo"); 
            break;
        }
        else {
            ejecutar_externo(args);
        }
    }

    return 0;
}
