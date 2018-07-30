#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/history.h>

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_history(char **args);
int lsh_issue(char **args);
int lsh_execute(char **);
int lsh_program(char **);
int lsh_rmexcept(char **);
char ** lsh_split_line(char *);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "history",
  "issue",
  "program",
  "rmexcept"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_history,
  &lsh_issue,
  &lsh_program,
  &lsh_rmexcept
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */

int lsh_rmexcept(char **args)
{
  int i=0,ctr = 0;
  for(i=0;args[i];i++)
    ctr++;
  int status;
  status = system("ls > abc.txt");
  FILE * fp;
  char *line = NULL;
  size_t len = 0;
      ssize_t read;
  fp = fopen("abc.txt", "r");
  ctr=0;
 while ((read = getline(&line, &len, fp)) != -1) {
      int j=1;
      int flag = 0;
      int l = strlen(line);
      line[l-1]='\0';
      while(args[j])
      {
        if(strcmp(args[j],line)==0)
        {
          flag = 1;
          break;
        }
        j++;
      }
      if(!flag && strcmp(line,"abc.txt")!=0 && strcmp(line,"shell.c")!=0 && strcmp(line,"a.out")!=0 && strcmp(line,"readme.txt")!=0 && strcmp(line,"makefile")!=0 )
      {
        char *str = (char*)malloc(100*sizeof(char));
        strcpy(str,"rm ");
        strcat(str,line);
        printf("%s\n", str);
        status = system(str);
      }
     }
    fclose(fp);
    return 1;
}

int lsh_program(char **args)
{
  if(args[1] && strcmp(args[1],">")!=0 && strcmp(args[1],"<")!=0)
  {
      char *str = (char*)malloc(100*sizeof(char));//timeout arg[1] arg[0]
    strcpy(str,"timeout ");
    int i;
    for(i=8;i<8 + strlen(args[1]); i++)
      str[i] = args[1][i-8];
    
    str[i++] = ' ';
    int len = strlen(args[1])+9+strlen(args[0]);
    for(;i<len; i++)
      str[i] = args[0][i-strlen(args[1])-9];
    // printf("%s\n",str);
   int status = system(str);
    return 1;
  }
  int j,len=0;
  char *str = (char*)malloc(100*sizeof(char));
  for(int i=0;args[i];i++)
  {
    for(j=0;args[i][j];j++)
      str[len++] = args[i][j];
    str[len++] =' ';
  }
  str[len++]='\0';

  int status = system(str);
  return 1;
}

int lsh_issue(char **args)
{
    int n = atoi(args[1]);
    register HIST_ENTRY **the_list;
    the_list = history_list ();
    if(!the_list)
    {
      printf("Empty List\n");
      return 1;
    }
    int ctr=0;
    int i;
    for(i=0;the_list[i];i++)
      ctr++;
    if(n>=ctr)
    {
      printf("Out of bound\n");
      return 1;
    }
    char *str = the_list[n-1]->line;
    if(str[0]=='i' && str[1]=='s' && str[2]=='s')
    {
      printf("Recursive Loop\n");
      return 1;
    }
    else{
      char **arguments = lsh_split_line(str);
      int status = lsh_execute(arguments);

    }
}


int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_history(char **args)
{
  register HIST_ENTRY **the_list;
  register int i;

  the_list = history_list ();
  if (the_list)
  {
    int ctr=0;
    for(i=0;the_list[i];i++)
      ctr++;
    
    int ctr2=0;
    for(i=0;args[i];i++)
      ctr2++;
    

    if(ctr2==1)
      for (i = 0; the_list[i]; i++)
        printf ("%d: %s\n", i + history_base, the_list[i]->line);
    else
    {

      int n = atoi(args[1]);
      for(i=ctr-1;i>=(ctr-n) && i>=0;i--)
      {
                printf ("%d: %s\n", i + history_base, the_list[i]->line);
      }
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      // if(args[0]=="history")
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

     
  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
   
      return (*builtin_func[i])(args);
    }
  }
  if(strcmp(args[0], "cd") != 0 && strcmp(args[0], "ls") != 0 && strcmp(args[0], "rm") != 0)
    return (*builtin_func[5])(args);
  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      add_history(buffer);
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.
  using_history ();
  printf("***Implementing a basic shell**\n");
  printf("Following commands are implemented under our shell:- \n");
  printf("\ncd <directory_name>:--- cd Desktop\n");
  printf("\nls:--- ls\n");
  printf("\nrm:--- rm a.txt \n");
  printf("\nhistory n:--- history 5 / history\n");
  printf("\nissue n:--- issue 5\n");
  printf("\n<program_name>:--- ./shell \n");
  printf("\nexit:--- exit\n");
  printf("\nrmexcept <list_of_files> :--- rmexcept shell.c a.out makefile print.c abc.txt\n");
  printf("\n<program_name> m :--- ./shell 0.02 \n\n");
  printf("-----------------------------------------------------\nEnter the command:\n");
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
