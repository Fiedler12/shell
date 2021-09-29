#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void freedom();
void tokenizer();
char ** tkn_command();
void handle_string();
void command();
char **recstring;
char buffer[80];
int child_pid;
void pipeExec();

/*
 * Gets the user input directly and checks if it has
 * pipe, if pipe is given it will split all commands
 * up - thereafter / or if no pipe , the function
 * will begin tokenizing the command(s) and put them
 * into the the command array which is an array of
 * arrays of strings.
 * */
void handle_string(char* line, char ***command_array)
{
    //removing newline characters
    size_t ln = strlen(line) - 1;
    if (*line&& line[ln] == '\n')
        line[ln] = '\0';

    //if pipe
    if (strstr(line,"|"))
    {
        char *commands[1024];
        tokenizer(line,"|",commands);
        int i = 0;
        for(int i=0; commands[i]!=NULL; i++)
        {
            command_array[i] = tkn_command(commands[i]);
        }
    }
        //if no pipe
    else
    {
        command_array[0] = tkn_command(line);
    }
}

/*
 * Allocates space for the string array which
 * holds the commands used by the shell.
 * */
char** tkn_command(char *line)
{
    char ** command = malloc(1024*sizeof(char*));
    for (int i = 0 ; i < 1024; ++i)
        command[i] = malloc(1024 * sizeof(char));
    tokenizer(line," ",command);
    return command;
}

/*
 * Takes a string and splits it up into parts
 * delimitted by a defined delimiter.
 * affects a pointer to a string array.
 * */
void tokenizer(char *command, char *delim, char *output[])
{
    int i=0;
    output[i] = strtok(command, delim);
    while(output[i] != NULL)
    {
        output[++i] = strtok(NULL, delim);
    }
}

/*
 * Freeing up all the arrays that have been
 * made in the tkn_command() function.
 * */
void freedom(char ***command_array)
{
    for(int i = 0; i>1024; i++){
        for(int j = 0; i>1024; i++){
            free(command_array[i][j]);
        }
        free(command_array[i]);
    }
}

void command(char **command_array[]) {
    //first we define a string that corresponds with our fork command to have a reference.
    if (command_array[1] == NULL) {
        child_pid = fork();
        if (child_pid == 0) {
            execvp(command_array[0][0], command_array[0]);
        }
        else {
            wait(NULL);
        }
    }
    else {
        pipeExec();
        }
    }

    void pipeExec(char **command_array[]) {
        int pipefd[2];
        int pid;
        pipe(pipefd);
        pid = fork();
        if (pid == 0) {
            recstring = (char **) command_array[0][0];
            dup2(pipefd[0], 0);
            close(pipefd[1]);
            write(pipefd[1], recstring, (strlen((const char *) recstring) + 1));
            execvp(command_array[0][0], command_array[0]);
            }
            else {
                dup2(pipefd[1], 1);
                close(pipefd[0]);
                printf("pipe recieved: %s \n", buffer);
            }
}

int main() {
    while (1)
    {
        int status;
        printf("Enter command: ");
        char line[1024] = "";
        char **command_array[1024] = {NULL};
        fgets(line, 1024, stdin);
        handle_string(line, command_array);
        command(command_array);
        freedom(command_array);
    }
    return 0;
}