#include <stdio.h>
#include <string.h>
#include "path_handle.h"

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
