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

// path handler
struct node {
	char *dir;
	struct node* next;
};

typedef struct node node;

node *create_path_list(char *path)
{
	node *head = NULL;
	node *tail = NULL;

	// parse the path and split into dirs
	char* dir = strtok(path, ":");
	
	while (dir != NULL)
	{
		// new node for dir
		node *new_node = (node*) malloc (sizeof(node));
		new_node->dir = dir;
		new_node->next = NULL;

		// add the new node to the list
		if (!head)
		{
			head = new_node;
			tail = new_node;
		} else 
		{
			tail->next = new_node;
			tail = new_node;
		}

		dir = strtok(NULL, ":");
	}

	return head;
}

int main(int argc, char** argv)
{
	qsh_loop();
	return 0;
}

void qsh_loop()
{
	do {
		if (isatty(STDIN_FILENO)) // interactive mode
		{
			qsh_prompt();			
			qsh_handle_input();
		} else // non-interactive mode
		{
			printf("Non-interactive mode");
		}
	} while(1);
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
