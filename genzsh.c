#include "genzsh.h"

//------------ Main Loop --------------//
int main(int argc, char **argv) {
    //load config files if any exist

    //Main loop
    mainLoop();

    //cleanup
    return EXIT_SUCCESS;
}

void mainLoop(void) {
    char * line;
    char ** args;
    int status;

    do {
        char CWD[PATH_MAX];
        if (getcwd(CWD, sizeof(CWD)) != NULL) {
            printf("It\'s giving %s> ", CWD);
        } else {
            printf("> ");
        }
        line = readLine();
        args = splitLine(line);
        status = execute(args);
        free(line);
        free(args);
    } while (status);
}



/**
 * reads a line of input from the standard input, e.g. a terminal emulator,
 * teletype, serial input, etc.
 * @return array of chars representing the input. Input is not split into args.
*/
#define RL_BUFSIZE 1024
char * readLine(void) {
    int bufsize = RL_BUFSIZE;
    int pos = 0;
    char * buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "genzsh: allocation is not bussin\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        } else {
            buffer[pos] = c;
        }
        ++pos;
    }

    if (pos >= bufsize) {
        bufsize += RL_BUFSIZE;
        buffer = realloc(buffer, bufsize);
        if (!buffer) {
            fprintf(stderr, "genzsh: allocation is not bussin\n");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * Splits a line into tokens separated by the listed delimiters.
 * @param line an array of characters representing a command and args
 * @return an array of strings, or a pointer to char pointers (C is fun) 
 */
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
char ** splitLine(char * line) {
    int bufsize = TOK_BUFSIZE, pos = 0;
    char ** tokens = malloc(bufsize * sizeof(char*));
    char * token;

    if (!tokens) {
        fprintf(stderr, "genzsh: allocation is not bussin\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[pos] = token;
        ++pos;

        if (pos >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "genzsh: allocation is not bussin\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

/**
 * Attempts to launch a process given a command and a list of arguments.
 * If the execution fails, an error is thrown from the child process; otherwise,
 * the parent process waits for output from the child.
 * @return the number 1 if nothing went wrong.
*/
int launchProc(char ** args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("genzsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("genzsh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

/**
 * Attempts to launch a process or builtin given a command and list of arguments.
 * Throws error from child process if it fails or waits for process to return if it is successful.
 * @return result of the process
*/
int execute(char ** args) {
    int i;
    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return launchProc(args);
}

//Builtin implementation

//Self explanatory
int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/**
 * Changes current working directory.
 * Requires an argument for the directory to enter.
 * @return a status code. Should always be 1 to return to the shell.
*/
int genz_cd(char ** args) {
    if (args[1] == NULL) {
        fprintf(stderr, 
            "genzsh: boutta catch these hands if you don\'t provide args for \'hit_different\'\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("genzsh");
        }
    }
    return 1;
}

/**
 * Returns an incredibly helpful message that totally describes the app without any ironic use of slangs at all.
*/
int genz_help(char ** args) {
    int i;
    printf("James Vogt\'s GENZSH\n");
    printf("\n");
    printf("This goated program lets you girlboss by telling the computer what to do.\n");
    printf("For a list of commands, read the README fr fr.\n");
    printf("If you\'re still shook, ask ChatGPT or smth smh.\n\n");
    printf("These commands are builtin AF:\n");

    for (i = 0; i < num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }

    printf("\nUse the man command and the computer will spill the tea about the other programs fr.\n");
    return 1;
}

/**
 * Exits shell.
*/
int genz_exit(char ** args) {
    return 0;
}

// Insanely basic echo command accessed with 'yap'.
// TODO: add escape sequence functionality, error checking and flags
int genz_echo(char ** args) {
    char ** ptr = args;
    int ignore_yap = 0;
    while (*ptr != 0) {
        if (ignore_yap == 0) {
            ignore_yap = 1;
            ptr++;
            continue;
        }
        printf("%s ", *ptr);
        ptr++;
    }
    printf("\n");
    return 1;
}