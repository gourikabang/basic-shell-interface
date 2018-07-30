# basic-shell-interface

As we have seen the system call interface that an Operating System provides to the  user  programs  to  interact  with  the  hardware  resources. 

The  shell interface  provides the  user  a  prompt  after  which  he  types  in  a  command, and  then the  shell creates a separate child process that executes the command. After execution, it prompts for more user inputs until  the  user  enters exit at  the  prompt. 

## Shell  is  able  to execute  the  following commands:
#### cd <directory_name>
	Changes current directory if user has appropriate permissions

#### ls
	Lists information about files in the current directory

#### rm
	Deletes indicated files. Supports options -r, -f, -v

#### history n
	Prints the most recent n commands issued by the numbers including the current history n command. 
	If n is ommitted, prints all commands issued by user.
	If we have 10 entries in history and user enters history 15 so all 10 will be shown.

#### issue n
	Issues the nth command in the history once again
	n is compulsory
	if history has 3 commands and next command is issue 4 so it can go into recursive loop, to avoid that we have taken appropriate measures
	if elements in history are 5 and user enters issue 10 so it is out of bound, regarding this also appropriate measures are taken

#### <program_name>
	Creates a child process to run <program_name>. 
	It supports redirection < and > to redirect the input and out of the program

#### exit
	Helps user exit the shell

#### rmexcept <list_of_files>
	Input for file names is space separated
	it take cares(built in) that current C file, its a.out and makefile is not deleted

#### <program_name> m
	m can be float value

### Limitations
    Commands must be on a single line.
    Arguments must be separated by whitespace.
    No quoting arguments or escaping whitespace.
    No piping
    Redirection implemented in few commands only
    In rmexcept command we have created a file abc.txt that stores the output of ls command so that we can proceed further.


### Steps to execute
##### Shell requires:
	sudo apt-get install libreadline6 libreadline6-dev
	sudo apt-get install coreutils
##### Compiling
	gcc shell.c -lreadline 
