#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdlib.h>

// Node representing a value in the queue.
struct Node {
    uint32_t data;
    struct Node* next;
};

// Queue structure.
typedef struct {
    struct Node *front, *rear;
} Queue;

// Function to create a new node.
struct Node* createNode(uint32_t data);

// Function to create a queue.
Queue* createQueue();

// Function to add an element to the queue.
void enqueue(Queue* q, uint32_t data);

// Function to remove an element from the queue.
uint32_t dequeue(Queue* q);

#endif // QUEUE_H