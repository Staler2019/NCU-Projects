#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* @Finished: 1
 *      a line string splits into commands,
 *      and each command splits into args in each loop for a line
 * @TODO: 2
 *      make up a pipeline
 * @TODO: 3
 *      make up a file redirection
 * @Purpose:
 *      you can simply know the function doing by its name
 */

#define SPLIT_BUFFER 4
#define TOKENS_DELIM " \t\r\n\a"
#define COMMANDS_DELIM "|"

enum _FioType {
    STDOUT2FILE_AND_STDERR2STDOUT,
    STDOUT2FILE_AND_STDERR2FILE,
    STDERR2STDOUT,
    STDOUT2FILE,
    STDERR2FILE,
    NORMALSTD,
    // TODO: difference beween stds
    APPEND2FILE_READFROMFILE,
    OVERWRITE2FILE_READFROMFILE,
    APPEND2FILE,
    OVERWRITE2FILE,
    READFROMFILE,
    NORMAL
};
typedef enum _FioType FIOTYPE;

void sh_print_prompt(void);
void sh_loop(void);
char **sh_read_line(char *_line);
char **sh_read_command(char *_command);
char **split_string(char *_str, char *_delim);
int sh_execute(char **args);

int main() {
    sh_loop();
    return EXIT_SUCCESS;
}

void sh_loop(void) {
    int status;
    char *line;
    char **commands;
    char **args;

    do {
        sh_print_prompt();
        // read line and split into commands
        commands = sh_read_line(line);

        for (int commands_index = 0; commands[commands_index] != NULL; commands_index++) {
            char *command = commands[commands_index];
            // make each command into args
            args = sh_read_command(command);
            status = sh_execute(args);

            if (status == 0) break;  // exit
        }

        // end loop
        free(line);
        free(commands);
        free(args);
    } while (status);
}

void sh_print_prompt() { printf("$ "); }

char **sh_read_line(char *_line) {
    // getline()
    ssize_t getline_buffer = 0;
    getline(&_line, &getline_buffer, stdin);

    char **commands = split_string(_line, COMMANDS_DELIM);
    return commands;
}

char **sh_read_command(char *_command) {
    char **args = split_string(_command, TOKENS_DELIM);
    return args;
}

char **split_string(char *_str, char *_delim) {
    int tokens_buffer = SPLIT_BUFFER;
    char **tokens = malloc(tokens_buffer * sizeof(char *));

    // make tokens
    char *token = strtok(_str, _delim);
    int tokens_index = 0;
    while (token != NULL) {
        // add token to tokens
        tokens[tokens_index++] = token;

        // check if malloc size is enough or not
        if (tokens_index >= tokens_buffer) {
            tokens_buffer <<= 1;
            tokens = realloc(tokens, tokens_buffer * sizeof(char *));
        }

        // find next token
        token = strtok(NULL, _delim);
    }
    tokens[tokens_index] = NULL;

    return tokens;
}

int sh_execute(char **args) {
    // my_sh commands
    if (args[0] == "cd") {
        char *dir = args[1];
        if (dir == NULL) {
            if (chdir(dir) != 0) {
                perror("error: dir does not exist");
                exit(EXIT_FAILURE);
            }
        }
    } else if (args[0] == "exit")
        return 0;
    else {
        // pipe
        int fd[2];
        if (pipe(fd) == -1) {
            perror("error: failed to create pipe");
            exit(EXIT_FAILURE);
        }

        // mormal_sh commands
        if (fork() != 0) {  // Parent code
            // int status; waitpid(-0, &status, 0);
            close(0);
            dup(fd[0]);
            close(fd[1]);
        } else {  // Child code
            close(1);
            dup(fd[1]);
            execvp(args[0], args);  // TODO: do the redirection
        }
    }
}