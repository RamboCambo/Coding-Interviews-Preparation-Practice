#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdlib.h>

// Node representing a value in the queue.
struct Node {
    struct RotateAndFactoriseArgs *data;
    struct Node* next;
};

// New data structure definition.
struct RotateAndFactoriseArgs {
    uint32_t number;
    uint32_t slot;
    char bits;
};

// Queue structure.
typedef struct {
    struct Node *front, *rear;
} Queue;

// Function to create a new node.
struct Node* createNode(struct RotateAndFactoriseArgs *data);

// Function to create a queue.
Queue* createQueue();

// Function to add an element to the queue.
void enqueue(Queue* q, struct RotateAndFactoriseArgs *data);

// Function to remove an element from the queue.
struct RotateAndFactoriseArgs *dequeue(Queue* q);

#endif // QUEUE_H
