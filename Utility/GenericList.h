#pragma once
#include <iostream>
#include <stdlib.h>


using namespace std;

typedef struct Node {
	void* data;
	struct Node* next;
};

void Init(Node** head) {
	*head = NULL;
}

void Prepend(Node** head, void* data, size_t size) {
	Node* node = (Node*)malloc(sizeof(Node));
	node->data = malloc(size);
	node->next = (*head);
	memcpy(node->data, data, size);
	(*head) = node;
}
void Print(Node* node, void(*ptr)(void*)) {
	while (node != NULL) {
		(*ptr)(node->data);
		node = node->next;
	}
}

void Destroy(Node** head) {
	if (*head == NULL) {
		return;
	}

	Node* next = (*head)->next;
	free((*head)->data);
	(*head)->data = NULL;
	(*head)->next = NULL;
	free(*head);
	*head = NULL;
	if (next != NULL) {
		Destroy(&next);
	}
}
bool DeleteNode(Node** head, void* selectedItem, size_t size) {
	Node* current = *head;
	if (current == NULL) {
		return false;
	}
	Node* prev = NULL;
	char* target = (char*)malloc(size);
	memcpy(target, selectedItem, size);
	char* data = (char*)malloc(size);
	while (current != NULL) {
		memcpy(data, current->data, size);
		if (*target == *data) {
			if (prev == NULL) {
				(*head) = current->next;
			}
			else {
				prev->next = current->next;
			}
			free(current->data);
			free(current);
			free(data);
			free(target);
			return true;
		}
		prev = current;
		current = current->next;
	}
	free(target);
	free(data);
	return false;
}