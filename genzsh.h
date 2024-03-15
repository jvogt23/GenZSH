#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main();
void mainLoop(void);
char * readLine(void);
char ** splitLine(char * line);
int launchProc(char ** args);
int execute(char ** args);

//Builtins
int genz_cd(char ** args);
int genz_help(char ** args);
int genz_exit(char ** args);
int num_builtins();

char *builtin_str[] = {
    "hit_different",
    "wtf",
    "periodt"
};

int (*builtin_func[]) (char **) = {
    &genz_cd,
    &genz_help,
    &genz_exit
};