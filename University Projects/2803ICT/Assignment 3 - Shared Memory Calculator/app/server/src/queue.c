#include "queue.h"

// Function to create a new node.
struct Node* createNode(struct RotateAndFactoriseArgs *data) {
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
void enqueue(Queue* q, struct RotateAndFactoriseArgs *data) {
    struct Node* temp = createNode(data);
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove an element from the queue and return the removed value.
struct RotateAndFactoriseArgs *dequeue(Queue* q) {
    if (q->front == NULL) {
        // In this case, we're returning a default struct with 0 values.
        return NULL;
    }
    struct Node* temp = q->front;
    struct RotateAndFactoriseArgs *item = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return item;
}
