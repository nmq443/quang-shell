#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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
void qsh_execute();

int x = 0;

int main(int argc, char** argv)
{
	qsh_loop();
	return 0;
}

void qsh_loop()
{
	//do {
		if (isatty(STDIN_FILENO)) // interactive mode
		{
			qsh_prompt();			
			qsh_handle_input();
		} else // non-interactive mode
		{
			printf("Non-interactive mode");
		}
	//} ;
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

void qsh_handle_input()
{
	char *line;
	char **args;

	line = qsh_read_line();
	// printf("%s\n", line);
	args = qsh_split_line(line);
	for (int i = 0; args[i] != NULL; i++)
	{
		printf("%s ", args[i]);
	}
	printf("\n");
}

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

void qsh_execute()
{

}
