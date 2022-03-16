#pragma once

#include <iostream>
#include "./Message.h"

using namespace std;

typedef struct LinkedList {
	struct LinkedList* next;
	Message value;
};

void Init(LinkedList** head) {
	*head = NULL;
}
void Append(struct LinkedList** head, Message m) {
	if (*head == NULL) {
		*head = (LinkedList*)malloc(sizeof(LinkedList));
		(*head)->next = NULL;
		(*head)->value = m;
		return;
	}
	LinkedList* temp = *head;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = (LinkedList*)malloc(sizeof(LinkedList));
	temp->next->next = NULL;
	temp->next->value = m;
}
void Print(LinkedList* head) {
	int index = 0;
	while (head != NULL) {
		cout << "Value " << ++index << ": " << head->value.value << endl;
		head = head->next;
	}
}
int Find(LinkedList* head, int value) {
	while (head != NULL) {
		if (head->value.value == value) {
			return 1;
		}
		head = head->next;
	}
	return 0;
}
void Prepend(LinkedList** head, Message m) {
	if (*head == NULL) {
		*head = (LinkedList*)malloc(sizeof(LinkedList));
		(*head)->next = NULL;
		(*head)->value = m;
		return;
	}
	LinkedList* node = (LinkedList*)malloc(sizeof(LinkedList));
	node->value = m;
	node->next = *head;
	*head = node;
}