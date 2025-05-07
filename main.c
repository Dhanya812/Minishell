/*
Name : DHANYA GUDIGAR
Date : 05/04/2025
Description : Implement Minishell
Sample input : ls
sample output : a.out  external_command.txt  main.c  msh.h  sample

Sample input : pwd
Sample output : /home/dhanya/LINUX_LAB/minishell
*/
#include"msh.h"  

int status, pid;  // Global variables to store process status and PID
//char opt;  // Unused variable (potential for command-line options)
char list[25];  // stores current command string
int j_num = 0;  // Job number counter
Slist *head = NULL;  // Head pointer for job list
char prompt[25] = "minishell$";  // Shell prompt string
char *external_command[200];  // Array to store external command names
int C_flag = 0;  // Flag for SIGINT
int Z_flag = 0;  // Flag for SIGTSTP

int main()  // Main function
{
        system("clear");  // Clear the terminal
        char input_string[25];  // Buffer to store user input
        scan_input(prompt, input_string);  // Call function to scan input
}

void insert_at_first(char *input_string, int pid)  // Insert job at start of job list
{
        Slist *new = malloc(sizeof(Slist));  // Allocate memory for new node
        if (new == NULL)
        {
                perror("memory allocation");  // Handle malloc failure
        }

        strcpy(new->command, input_string);  // Copy command to node
        new->pid = pid;  // Set process ID
        new->link = head;  // Link new node to existing list
        head = new;  // Update head to new node

        printf("\n[%d]+\tStopped\t\t%s\n", j_num++, new->command);  // Print job info
}

void delete_command_from_list()  // Remove first job from list
{
        if (head == NULL)
        {
                return;  // Do nothing if list is empty
        }
        Slist *temp = head;  // Store current head
        head = head->link;  // Move head to next node
        free(temp);  // Free old head node
}

void my_handler(int signum)  // Signal handler function
{
        if (signum == SIGINT)
        {
                printf("\n%s ", prompt);  // Print prompt again
                C_flag = 1;  // Set SIGINT flag
        }
        else if (signum == SIGTSTP)
        {
                insert_at_first(list, pid);  // Add command to job list
                printf("\n%s ", prompt);  // Print prompt again
                Z_flag = 1;  // Set SIGTSTP flag
        }
}

void scan_input(char *prompt, char *input_string)  // Handle user input
{
        extract_external_commands(external_command);  // Load external commands
        signal(SIGINT, my_handler);  // Register SIGINT handler
        signal(SIGTSTP, my_handler);  // Register SIGTSTP handler
label:
        while (1)  // Infinite loop
        {
                char copy[25] = {0};  // Temporary copy buffer

                if (!C_flag || !Z_flag)  // If no signals caught
                {
                        printf("%s", prompt);  // Print prompt
                }
		

                memset(input_string, 0, sizeof(input_string));  // Clear input buffer
                scanf(" %[^\n]", input_string);  // Read input string

                if(strncmp(input_string, "PS1", 3) == 0)  // Check if prompt change command
                {
                        for (int i = 0; input_string[i] != '\0'; i++)
                        {
                                if (input_string[i] == ' ')
                                {
                                        printf("ERROR : Command not found\n");  // Invalid format
                                        goto label;  // Go back to prompt
                                }
                        }
                        int i = 4, j = 0;  // Start after "PS1="
                        while (input_string[i] != '\0')
                        {
                                copy[j++] = input_string[i++];  // Copy new prompt
                        }
                        copy[j] = '\0';  // Null-terminate
                        strcpy(prompt, copy);  // Set new prompt
                        memset(input_string, 0, sizeof(input_string));  // Clear input buffer
                }
                else
                {
                        char *cmd = get_command(input_string);  // Extract command
                        int res = check_command_type(cmd);  // Check command type

                        if (res == BUILTIN)  // Handle built-in
                        {
                                execute_internal_commands(input_string);
                        }
                        else if (res == EXTERNAL)  // Handle external
                        {
                                strcpy(list, input_string);  // Save command string
                                pid = fork();  // Fork new process
                                if (pid > 0)
                                {
                                        waitpid(pid, &status, WUNTRACED);  // Wait and detect stop
                                        if (WIFSTOPPED(status)) {
                                                insert_at_first(input_string, pid);  // Add to jobs
                                        }
                                }
                                else if (pid == 0)
                                {
                                        signal(SIGINT, SIG_DFL);  // Default SIGINT
                                        signal(SIGTSTP, SIG_DFL);  // Default SIGTSTP
                                        execute_external_commands(input_string);  // Run command
                                }
                        }
                        else if (res == NO_COMMAND)
                        {
                                printf("no command\n");  // Handle unknown command
                        }
                }
        }
}

char *get_command(char *input_string)  // Extract command from input
{
        static char ip[25];  // Static buffer
        strcpy(ip, input_string);  // Copy input string
        char *command = strtok(ip, " ");  // Get first word
        return command;  // Return command
}

int check_command_type(char *cmd)  // Determine command type
{
        char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval", "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source", "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "fg", "bg", "jobs", NULL};  // List of builtins
        for (int i = 0; builtins[i] != NULL; i++)
        {
                if (strcmp(cmd, builtins[i]) == 0)
                {
                        return BUILTIN;  // Match found
                }
        }
        for (int j = 0; external_command[j] != NULL; j++)
        {
                if (strcmp(cmd, external_command[j]) == 0)
                {
                        return EXTERNAL;  // External match
                }
        }
        return NO_COMMAND;  // No match
}

void extract_external_commands(char **external_command)  // Load external commands
{
        int fd = open("external_command.txt", O_RDONLY);  // Open file
        if (fd == -1)
        {
                perror("ERROR : file cannot open\n");  // Handle error
                exit(0);
        }
        for (int i = 0; i < 153; i++)
        {
                external_command[i] = (char *)malloc(25 * sizeof(char));  // Allocate memory
        }
        int j = 0, i = 0;
        char ch;
        while (read(fd, &ch, 1) > 0)  // Read character by character
        {
                if (ch == '\n')  // End of command
                {
                        external_command[i][j] = '\0';  // Null-terminate
                        j = 0;
                        i++;
                }
                else
                {
                        external_command[i][j++] = ch;  // Store character
                }
        }
}

void execute_internal_commands(char *input_string)  // Run built-in command
{
        if (strcmp(input_string, "exit") == 0)
        {
                exit(0);  // Exit shell
        }
        else if (strcmp(input_string, "pwd") == 0)
        {
                char buf[50];
                printf("%s\n", getcwd(buf, 50));  // Print current dir
        }
        else if (strcmp(input_string, "echo $$") == 0)
        {
                printf("pid id %d\n", getpid());  // Print shell PID
        }
        else if (strcmp(input_string, "echo $?") == 0)
        {
                printf("exit status %d\n", WEXITSTATUS(status));  // Print exit status
        }
        else if (strcmp(input_string, "echo $SHELL") == 0)
        {
                printf("%s\n", getenv("SHELL"));  // Print shell path
        }
        else if (strncmp(input_string, "cd", 2) == 0)
        {
                char *token = strtok(input_string, " ");
                token = strtok(NULL, " ");
                if (chdir(token) == 0)
                {
                        printf("%s\n", token);  // Change directory
                }
        }
        else if (strcmp(input_string, "jobs") == 0)
        {
                if (head == NULL)
                {
                        printf("no jobs\n");  // No jobs
                }
                else
                {
                        Slist *temp = head;
                        int job_n = 1;
                        while (temp)
                        {
                                printf("[%d]+\t\t%s\n", job_n++, temp->command);  // Print jobs
                                temp = temp->link;
                        }
                }
        }
        else if (strcmp(input_string, "bg") == 0)
        {
                if (head != NULL)
                {
                        kill(head->pid, SIGCONT);  // Resume in background
                        printf("[%d]+ %s &\n", head->pid, head->command);
                        delete_command_from_list();  // Remove job
                }
                else
                {
                        printf("no job to resume in the background\n");
                }
        }
        else if (strcmp(input_string, "fg") == 0)
        {
                if (head != NULL)
                {
                        printf("^%s\n", head->command);  // Resume in foreground
                        kill(head->pid, SIGCONT);
                        waitpid(head->pid, NULL, 0);
                        delete_command_from_list();
                }
                else
                {
                        printf("no job to bring to the foreground\n");
                }
        }
}

void execute_external_commands(char *input_string)  // Run external commands with or without pipes
{
        printf("external command\n");  // Debug message to show execution reached here
        char str[25];  // Temporary string to hold a copy of the input
        strcpy(str, input_string);  // Copy input string to avoid modifying original
        char *argv[25];  // Array to hold tokenized arguments
        int j = 0;  // Index for argv

        char *token = strtok(str, " ");  // Tokenize input string by spaces
        while (token != NULL)  // Loop until all tokens are extracted
        {
                argv[j++] = token;  // Store token in argv
                token = strtok(NULL, " ");  // Get next token
        }
        argv[j] = NULL;  // Null-terminate the argument list (required by execvp)

        // Variables to handle pipes
        int flag = 0, pipe_count = 0, cmd_pos[10], count = 0;

        for (int i = 0; argv[i] != NULL; i++)  // Loop through all arguments
        {
                if (strcmp(argv[i], "|") == 0)  // Check if current token is a pipe
                {
                        flag = 1;  // Set flag that piping is needed
                        pipe_count++;  // Count number of pipes
                        argv[i] = NULL;  // Replace "|" with NULL to split commands
                }
                else if ((i == 0) || (argv[i - 1] == NULL))  // Start of a new command
                {
                        cmd_pos[count++] = i;  // Record the index of new command
                }
        }

        if (flag == 0)  // If no pipes are present
        {
                execvp(argv[0], argv);  // Directly execute the command
                perror("execvp failed");  // If execvp fails, print error
                exit(EXIT_FAILURE);  // Exit with failure status
        }
        else  // If piping is involved
        {
                int std_in = dup(0);  // Backup standard input
                int std_out = dup(1);  // Backup standard output
                int argc = pipe_count + 1;  // Number of commands = pipes + 1
                int fd[2], ret, pid;  // Pipe file descriptors, return value, and process ID

                for (int i = 0; i < argc; i++)  // Loop through all commands in pipeline
                {
                        if (i < pipe_count)  // Only create pipe if not the last command
                        {
                                ret = pipe(fd);  // Create pipe
                                if (ret == -1)
                                {
                                        perror("pipe");  // Print error if pipe creation fails
                                        exit(0);  // Exit on failure
                                }
                        }

                        pid = fork();  // Fork a new process
                        if (pid > 0)  // Parent process
                        {
                                if (i < pipe_count)
                                {
                                        close(fd[1]);  // Close write end
                                        dup2(fd[0], 0);  // Redirect read end to stdin
                                        close(fd[0]);  // Close original read end
                                }
                        }
                        else if (pid == 0)  // Child process
                        {
                                signal(SIGINT, SIG_DFL);  // Reset Ctrl+C to default
                                signal(SIGTSTP, SIG_DFL);  // Reset Ctrl+Z to default
                                if (i < pipe_count)
                                {
                                        close(fd[0]);  // Close read end
                                        dup2(fd[1], 1);  // Redirect stdout to write end of pipe
                                        close(fd[1]);  // Close original write end
                                }

                                execvp(argv[cmd_pos[i]], argv + cmd_pos[i]);  // Execute each command with correct args
                                perror("execvp");  // Print error if command fails
                                exit(EXIT_FAILURE);  // Exit on failure
                        }
                }

                waitpid(pid, NULL, 0);  // Wait for last child process to complete
                dup2(std_in, 0);  // Restore original stdin
                dup2(std_out, 1);  // Restore original stdout
        }
}
