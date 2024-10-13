 
The **Simple Shell Interpreter (SSI)** is designed to replicate basic functionalities of a Unix shell. The SSI allows users to execute commands both in the foreground and background, navigate directories, and manage background processes. It supports standard commands like **ls** and **cd**,  and custom features such as background execution (**bg**), listing active background processes (**bglist**).

## Part 1: Foreground Execution 
* Displays proper prompt format.
* Runs commands with fork() and execvp().

## Part 2:
* can change directories with cd.
	* cd .. 
	* cd ~
	* cd
	* cd a/b/c
* all supported. 
* PWD.
* shows in prompt CWD.

## Part 3:
* bg commands runs background processes. 
* bg list displays current list of background processes.
* user notified after each process completion.  


# 

My UNIX SSI should implement all required features while freeing all used memory.