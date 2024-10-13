#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

//NAME: Shay Van Pelt

struct bg_pro {
    pid_t pid;             /* pid for struct */
    char *command;         /* command for struct */
    struct bg_pro *next;   /* next in struct */
};

struct bg_pro* root; //root of struct
int bg_count;       //number of structs

//part 1 of assignment 
void part1(char** args)
{
    pid_t pid;  
    pid = fork(); //fork into child and parent
    if (pid < 0) 
    {
        perror("Error creating fork");
    }
    else if (pid == 0) //child
    {
        if (execvp(args[0], args) == -1) //execute child 
        {
            printf("%s: %s\n", args[0], strerror(errno));
            exit(1); 
        }
    }
    else //parent
    {
        if (wait(NULL) == -1) //wait for child to finish
        {
            perror("Error in parent\n");
        }
    }
}

//part 2 of assignment
void part2(char** args)
{
    char* home_path = getenv("HOME"); //path to home enviroment 

    if (args[1]==NULL || strcmp(args[1],"~") == 0) //if cd or cd ~ 
    {
        if (chdir(home_path) == -1)
        {
            printf("%s: %s\n", home_path, strerror(errno)); //print if chdir fails
        }
    }
    else 
    {
        if (args[1][0] == '~') //if starts with ~
        {
            char new_path[500] = "";
            int length = strlen(args[1]); //length of command
            strcat(new_path,home_path);  //cpy home path to new string
            for (int i = 1; i < length; i++) 
            {
                int path_length = strlen(new_path); //length of current new path
                new_path[path_length] = args[1][i]; //add character by character 
                new_path[path_length + 1] = '\0'; //add NULL

            }
            args[1] = new_path;
        }
        if (chdir(args[1]) == -1)
        {
            printf("%s: %s\n",args[1], strerror(errno)); //print if chdir fails
        }
    }
}

//part 3 of assignment 
void part3(char** args, int numTokens)
{
    char* newArgs[500]; //array to be left shifted
    char com[500] = ""; //string command
    int newTokens = 0; 
    while(newTokens < (numTokens-1))
    {
        newArgs[newTokens] = args[newTokens+1];
        strcat(com, newArgs[newTokens]);
        strcat(com, " "); 
        newTokens++;
    }
    newArgs[newTokens] = NULL; //left shifted new array of argv
    pid_t pid;
    pid = fork(); //fork into child and parent
    if (pid < 0)
    {
        perror("Error creating fork");
    }
    else if (pid == 0) //child
    {
        //make strout and strerr print to /dev/null
        int dev_null = open("/dev/null", O_WRONLY);
        if (dev_null == -1) 
        {
            perror("error opining dev_null");
            exit(1);
        }
        if (dup2(dev_null, STDOUT_FILENO) == -1) 
        {
            perror("error dup2 stdout");
            exit(1);
        }
        if (dup2(dev_null, STDERR_FILENO) == -1) 
        {
            perror("error dup2 stderr");
            exit(1);
        }
        close(dev_null);

        if (execvp(newArgs[0], newArgs) == -1) //execute child 
        {
            printf("%s: %s\n", newArgs[0], strerror(errno));
            exit(1); 
        }
    }
    else //parent
    {
        struct bg_pro* new = (struct bg_pro*) malloc(sizeof(struct bg_pro)); //create new struct 
        new->pid = pid; //assign pid
        new->command = strdup(com); //assign command
        new->next  = NULL; //assign nect 
        if (bg_count == 0){
            root = new; //if no current struct make = root
        }else{
            struct bg_pro *cur = root; 
            while (cur->next != NULL) //cycle through struct untill NULL
            {
                cur = cur->next; //last one would be end of struct
            }
            cur->next = new;
        }
        bg_count++; //update number of bg in struct
    }
}

int main()
{
    root = NULL;  
    bg_count = 0; //0 bg in struct

    int cdChanged = 0; //if path chnaged
    char prompt[500] = ""; 
    char* username = getlogin(); //get login

    char hostname[100];   
    if (gethostname(hostname, sizeof(hostname)) == -1) //get hostname
    {
        perror("error get hostname");  
        exit(1);
    }

    char* temp = "@";
    strcat(username,temp); //assign @ in between username and hostname
    strcat(username,hostname);
    temp = ": "; 
    strcat(username,temp); //assign : after hostname
    strcat(prompt,username); //assign to prompt 
    char tempPrompt[500] = "";
    strcpy(tempPrompt, prompt); //make a copy
    
	while (1)
	{
        if (cdChanged == 0) //if path changed or when first enter loop
        {
            char cwd[500];
            if (getcwd(cwd, sizeof(cwd)) == NULL)  //get cwd
            {
                perror("error get cwd");  
                exit(1);
            }
            strcpy(tempPrompt, prompt); //make copy
            strcat(tempPrompt,cwd); //add cwd
            temp = " > ";
            strcat(tempPrompt,temp); //add > 
            cdChanged = 1; //reset cdchanaged
        }

        // read and print complete line
		char* reply = readline(tempPrompt);

        //make sure "" does nothing
        if(strcmp(reply, "") != 0)
        {
            int numTokens = 0; //number of tokens
            char* args[500]; //args in input
            args[0] = strtok(reply, " \n");
            while(args[numTokens] != NULL)
            {
                args[numTokens+1]=strtok(NULL," \n"); //tokenize input from reply
                numTokens++; //increase number of token
            }

            //exit
            if (strcmp(args[0], "exit") == 0)
            {
                printf("exit\n"); 
                return 0; //if exit, exit()
            }
            //part 2 
            else if (strcmp(args[0], "cd") == 0)
            {
                part2(args); //do part 2 
                cdChanged = 0; //cwd chanaged 
            }
            //part 3
            else if (strcmp(args[0], "bg") == 0)
            {
                part3(args, numTokens); //do part 3
            }
            //bg list
            else if (strcmp(args[0], "bglist") == 0)
            {
                struct bg_pro *cur = root; 
                while(cur != NULL)
                {
                    printf("%d: %s\n", cur->pid, cur->command);  //print pid and command 
                    cur = cur->next; //next in struct 
                }
                printf("Total Background jobs:  %d\n", bg_count); //print number background jobs
            }
            //part 1
            else
            {
                part1(args); //do part1
            }
        }
        //check if child terminates
        if (bg_count > 0)
        {
            pid_t ter = waitpid(0,NULL,WNOHANG); //wait for child to finish
            while (ter > 0) //if child terminates
            {
                if (ter > 0) //if child terminates
                {
                    if (root->pid == ter) //if root terminated
                    {
                        printf("%d: %s has terminated.\n", root->pid, root->command); //print root pid has terminated
                        struct bg_pro *temp = root; //allow root to be free'd
                        root = root->next; 
                        free(temp->command); //free root command
                        free(temp); //free root
                        bg_count--; //decrease bg count
                    }
                    else
                    {
                        struct bg_pro *cur = root;
                        struct bg_pro *temp = NULL;
                        while (cur->next != NULL)
                        {
                            if (cur->next->pid == ter)
                            {
                                printf("%d: %s has terminated.\n", cur->next->pid, cur->next->command); //print pid has terminated
                                temp = cur->next;
                                cur->next = cur->next->next;
                                free(temp->command); //free command
                                free(temp); //free struct
                                bg_count--; //decrease bg count
                                break;
                            }
                            cur = cur->next;
                        }
                    }
                }
                ter = waitpid(0,NULL,WNOHANG); //wait for child to finish
            }
        }
        free(reply); //free input 
	}
	return 0;
}
