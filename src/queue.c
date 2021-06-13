#include <err.h>
#include <stdlib.h>
#include ".h/queue.h"

queue* queue_push(queue* start, int x, int y)
{
    struct queue *tmp;
    tmp = malloc(sizeof(struct queue));
    tmp->x = x;
    tmp->y = y;
    if(start){
        tmp->next = start->next;
        start->next = tmp;
    }
    else
        tmp->next = tmp;
   return tmp;
}

queue* queue_pop(queue* start, int *x, int *y)
{
    struct queue *tmp = start->next;
    *x = tmp->x;
    *y = tmp->y;
    if(tmp == tmp->next)
        start = NULL;
    else
        start->next = tmp->next;
    //free(tmp);
    return start;
}

void queue_empty(queue** pstart)
{
    while(*pstart == (*pstart)->next){
        queue_pop(*pstart, NULL, NULL);
    }
}
