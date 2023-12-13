#include "queue.h"

// Function to create a new node.
struct Node* createNode(uint32_t data) {
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    if(temp == NULL) {
        return NULL;
    }
    temp->data = data;
    temp->next = NULL;
    return temp;
}

// Function to create a queue.
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if(q == NULL) {
        return NULL;
    }
    q->front = q->rear = NULL;
    return q;
}

// Function to add an element to the queue.
void enqueue(Queue* q, uint32_t data) {
    struct Node* temp = createNode(data);
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove an element from the queue and return the removed value.
uint32_t dequeue(Queue* q) {
    if (q->front == NULL) {
        return 0; // Queue is empty
    }
    struct Node* temp = q->front;
    uint32_t item = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return item;
}