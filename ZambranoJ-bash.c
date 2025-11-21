#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX 256

void run_ls() {
    DIR *d = opendir(".");
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        write(1, e->d_name, strlen(e->d_name));
        write(1, "\n", 1);
    }
    closedir(d);
}

void run_dir() {
    DIR *d = opendir(".");
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        write(1, e->d_name, strlen(e->d_name));
        write(1, "\n", 1);
    }
    closedir(d);
}

void run_pwd() {
    char p[1024];
    getcwd(p, sizeof(p));
    write(1, p, strlen(p));
    write(1, "\n", 1);
}

void run_cd(char *d) {
    if (!d) return;
    chdir(d);
}

void run_mkdir(char *d) {
    mkdir(d, 0777);
}

void run_rm(char *d) {
    unlink(d);
}

void run_mv(char *a, char *b) {
    rename(a, b);
}

void run_cp(char *a, char *b) {
    int f = open(a, O_RDONLY);
    int g = open(b, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char buf[1024];
    int r;
    while ((r = read(f, buf, sizeof(buf))) > 0) write(g, buf, r);
    close(f);
    close(g);
}

void run_cat(char *f) {
    int fd = open(f, O_RDONLY);
    char buf[1024];
    int r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) write(1, buf, r);
    close(fd);
}

int parse(char *in, char **a) {
    int i = 0;
    a[i] = strtok(in, " \n");
    while (a[i] && i < MAX - 1) a[++i] = strtok(NULL, " \n");
    return i;
}

void ejecutar(char **a, int bg, char *rin, char *rout) {
    pid_t p = fork();
    if (p == 0) {
        if (rin) {
            int f = open(rin, O_RDONLY);
            dup2(f, 0);
            close(f);
        }
        if (rout) {
            int f = open(rout, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(f, 1);
            close(f);
        }
        execvp(a[0], a);
        exit(1);
    }
    if (!bg) waitpid(p, NULL, 0);
}

int main() {
    char in[MAX];
    char *a[MAX];

    while (1) {
        write(1, "BASH> ", 6);

        if (!fgets(in, sizeof(in), stdin)) break;

        int n = parse(in, a);
        if (n == 0) continue;

        int bg = 0;
        char *rin = NULL;
        char *rout = NULL;

        for (int i = 0; i < n; i++) {
            if (!strcmp(a[i], "&")) { bg = 1; a[i] = NULL; break; }
            if (!strcmp(a[i], "<")) { rin = a[i+1]; a[i] = NULL; break; }
            if (!strcmp(a[i], ">")) { rout = a[i+1]; a[i] = NULL; break; }
        }

        if (!strcmp(a[0], "exit")) break;
        else if (!strcmp(a[0], "ls")) run_ls();
        else if (!strcmp(a[0], "dir")) run_dir();
        else if (!strcmp(a[0], "pwd")) run_pwd();
        else if (!strcmp(a[0], "cd")) run_cd(a[1]);
        else if (!strcmp(a[0], "mkdir")) run_mkdir(a[1]);
        else if (!strcmp(a[0], "rm")) run_rm(a[1]);
        else if (!strcmp(a[0], "mv")) run_mv(a[1], a[2]);
        else if (!strcmp(a[0], "cp")) run_cp(a[1], a[2]);
        else if (!strcmp(a[0], "cat")) run_cat(a[1]);
        else ejecutar(a, bg, rin, rout);
    }
}
