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
int bytes;

/*
 * Gets the user input directly and checks if it has
 * pipe, if pipe is given it will split all commands
 * up - thereafter / or if no pipe , the function
 * will begin tokenizing the command(s) and put them
 * into the the command array which is an array of
 * arrays of strings.
 * */
void handle_string(char* line, char ***command_array)
/*
 * @author TA Martin Maartenson
 */
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
/*
 * @author TA Martin Maartenson
 */
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
/*
 * @author TA Martin Maartenson
 */
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
/*
 * @author TA Martin Maartenson
 */
{
    for(int i = 0; i>1024; i++){
        for(int j = 0; i>1024; i++){
            free(command_array[i][j]);
        }
        free(command_array[i]);
    }
}


void command(char **command_array[]) {
    /*
     * This command is called every time we get a string
     */
    //First we check whether there is several commands, which would mean a pipe is present
    if (command_array[1] == NULL) {
        //If not, we fork to create a child process.
        child_pid = fork();
        if (child_pid == 0) {
            //The child execs the command
            execvp(command_array[0][0], command_array[0]);
        }
        else {
            //The parent waits
            wait(NULL);
        }
    }
    else {
        //If we have a pipe in the command we jump to here.
        pipeExec();
        }
    }

    void pipeExec(char **command_array[]) {
    //we create our pipe and fork.
        int pipefd[2];
        //Creating pipe
        pipe(pipefd);
        int pid1 = fork();
        if (pid1 == 0) {
            //child process 1
            //Duplicate our STDOUT to our Pipe
            dup2(pipefd[1], STDOUT_FILENO);
            //We close our read end of the pipe
            close(pipefd[0]);
            //Exec the first command
            execvp(command_array[0][0], command_array[0]);
            int pid2 = fork();
            if (pid2 == 0) {
                //child process 2
                //Duplicate our STDIN to our pipe.
                dup2(pipefd[0], STDIN_FILENO);
                //We close the write end og our pipe
                close(pipefd[1]);
                // Exec the second command
                execvp(command_array[1][0], command_array[1]);
            }
            else {
                //Child process 1 wait.
                wait(NULL);
            }
            }
            else {
                //Parent wait.
                wait(NULL);

            }
}

int main() {
    while (1)
    {
        // @Authur TA Martin Maartenson
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