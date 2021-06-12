#include <err.h>
#include <stdlib.h>
#include ".h/shared_queue.h"

shared_queue* shared_queue_new()
{
    struct shared_queue *shared_q;

    if((shared_q = malloc(sizeof(shared_queue))) == NULL)
        errx(EXIT_FAILURE, "couldn't initialize the shared queue.");

    if((shared_q->queue = malloc(sizeof(queue))) == NULL)
        errx(EXIT_FAILURE, "couldn't initialize the queue.");

    shared_q->size = 0;

    return shared_q;
}

void shared_queue_push(shared_queue* sq, int x, int y)
{
    sq->size += 1;
    queue_push(sq->queue, x, y);
}

queue* shared_queue_pop(shared_queue* sq, int *x, int *y)
{
    queue *q = queue_pop(sq->queue, x, y);

    sq->size -= 1;

    return q;
}

void shared_queue_destroy(shared_queue* sq)
{
    queue_empty(&sq->queue);
    free(sq->queue);
    free(sq);
}
