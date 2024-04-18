#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

// main function
void qsh_loop();

// prompt
void qsh_prompt();

// input
void qsh_handle_input();

// read input
char *qsh_read_line();

// parse input
char **qsh_split_line(char *line);

// execute
int qsh_execute(char **args);
int qsh_launch(char **args);

// built-in commands
// cd, exit, help 
int qsh_cd(char** args);
int qsh_help(char** args);
int qsh_exit(char** args);
char* builtin_cmds[] = 
{
	"cd", 
	"help",
	"exit"
};

int (*builtin_func[]) (char**) = 
{
	&qsh_cd,
	&qsh_help,
	&qsh_exit
};

int qsh_num_builtins()
{
	return sizeof(builtin_cmds) / sizeof(char*);
}


int main(int argc, char** argv)
{
	qsh_loop();
	return 0;
}

void qsh_loop()
{
	char *line;
	char **args;
	int status;

	do {
		if (isatty(STDIN_FILENO)) // interactive mode
		{
			qsh_prompt();			
			//qsh_handle_input();
			line = qsh_read_line();
			args = qsh_split_line(line);
			status = qsh_execute(args);

			free(line);
			free(args);
		} else // non-interactive mode
		{
			printf("Non-interactive mode");
		}
	} while(status);
}

char* qsh_read_line()
{
	ssize_t bufsize = 1024;
	char *line = NULL;

	if (getline(&line, &bufsize, stdin) == -1)
	{
		if (feof(stdin))
		{
			exit(EXIT_SUCCESS);
		} else
		{
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}

	return line;
}

void qsh_prompt()
{
	int bufsize = 1024;
	char *cwd = malloc(bufsize * sizeof(char));
	char *hostname;
	char *username;

	// get current working directory
	getcwd(cwd, bufsize*sizeof(char));

	if (!cwd)
	{
		fprintf(stderr, "Allocation error!\n");
		exit(EXIT_FAILURE);
	} 

	// username and hostname
	username = getlogin();
	hostname = malloc (bufsize * sizeof(char));
	gethostname(hostname, sizeof(hostname));

	// print the prompt
	printf("%s@%s:%s ", username, hostname, cwd);

	free(cwd);
	//free(username);
	free(hostname);
}

/*
void qsh_handle_input()
{
	char *line;
	char **args;

	line = qsh_read_line();
	args = qsh_split_line(line);
}
*/

#define QSH_TOK_BUFSIZE 512
#define QSH_TOK_DELIM " \t\r\n\a"
char **qsh_split_line(char *line)
{
	int bufsize = QSH_TOK_BUFSIZE;
	char** tokens = malloc (bufsize * sizeof(char*));
	char* token;
	int position = 0;

	if (!tokens)
	{
		fprintf(stderr, "Allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, QSH_TOK_DELIM);

	while (token != NULL)
	{
		tokens[position++] = token;

		if (position >= bufsize)
		{
			bufsize += QSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens)
			{
				fprintf(stderr, "Allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, QSH_TOK_DELIM);
	}

	tokens[position] = NULL;
	return tokens;
}

int qsh_cd(char **args)
{
	if (!args[1])
	{
		fprintf(stderr, "qsh: expected argument to \"cd\"\n");
		exit(EXIT_FAILURE);
	} else 
	{
		if (chdir(args[1]))
		{
			perror("qsh");
		}
	}
	return 1;
}

int qsh_help(char **args)
{
	int i;
	printf("My shell\n");
	printf("Built-in commands: \n");

	for (int i = 0; i < qsh_num_builtins(); i++)
	{
		printf(" %s\n", builtin_cmds[i]);
	}

	printf("Use the man command for information on other programs\n");
	return 1;
}

int qsh_exit(char **args)
{
	return 0;
}

int qsh_launch(char **args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0)
	{
		// child process
		if (execvp(args[0], args) == -1)
		{
			perror("qsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0)
	{
		// error forking
		perror("qsh");
	} else 
	{
		// parent process
		do 
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

int qsh_execute(char **args)
{
	if (!args[0])
	{
		// empty command
		return 1;
	}

	for (int i = 0; i < qsh_num_builtins(); i++)
	{
		if (strcmp(args[0], builtin_cmds[i]) == 0)
		{
			return (*builtin_func[i])(args);
		}
	}

	return qsh_launch(args);
}
