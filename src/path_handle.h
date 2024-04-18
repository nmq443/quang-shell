#ifndef PATH_HANDLE_H
#define PATH_HANDLE_H

// path handler
struct node {
	char *dir;
	struct node* next;
};

typedef struct node node;

node *create_path_list(char *path);

#endif
