#define _CRT_SECURE_NO_WARNINGS

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct Node
{
    void* data;
    HANDLE mutex;
    struct Node* next;
};

void Init(Node** head) {
    *head = NULL;
    (*head)->mutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

}

void Prepend(Node** head_ref, void* new_data, size_t data_size)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    new_node->data = malloc(data_size);
    new_node->next = (*head_ref);
    new_node->mutex = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    memcpy(new_node->data, new_data, data_size);
    (*head_ref) = new_node;
}

void Print(Node* node, void (*fptr)(void*))
{
    DWORD dwWaitResult;
    while (node != NULL)
    {
        dwWaitResult = WaitForSingleObject(
            node->mutex,
            INFINITE);  
        if (dwWaitResult == WAIT_OBJECT_0) {  
            (*fptr)(node->data);
            node = node->next;
            ReleaseMutex(node->mutex);
        }
    }
}


void Destroy(Node** head) {
    if (*head == NULL) {
        return;
    }

    Node* next = (*head)->next;
    DWORD dwWaitResult = WaitForSingleObject(
        (*head)->mutex,    // handle to mutex
        INFINITE);  // no time-out interval
    if (dwWaitResult == WAIT_OBJECT_0) {
        CloseHandle((*head)->mutex);
        free((*head)->data);
        (*head)->data = NULL;
        (*head)->next = NULL;
        free(*head);
        *head = NULL;
        //release mutex on null??
    }

    if (next != NULL) {
        Destroy(&next);
    }
}


bool DeleteNode(Node** head, void* toDelete, size_t size) {
    Node* current = *head;
    if (current == NULL) {
        return false;
    }

    Node* prev = NULL;
    char* target = (char*)malloc(size);
    memcpy(target, toDelete, size);
    char* data = (char*)malloc(size);
    while (current != NULL) {
        DWORD dwWaitResult = WaitForSingleObject(
            (current)->mutex,    // handle to mutex
            INFINITE);  // no time-out interval
        if (dwWaitResult == WAIT_OBJECT_0) {
            memcpy(data, current->data, size);
            if (*target == *data) {
                //printf("found\n");

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
            ReleaseMutex(current->mutex);
            CloseHandle(current->mutex);
            prev = current;
            current = current->next;
        }

    }
    free(target);
    free(data);
    return false;
}