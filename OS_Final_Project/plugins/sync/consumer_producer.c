#include <stdlib.h>
#include <string.h>
#include "consumer_producer.h"

const char* consumer_producer_init(consumer_producer_t* queue, int capacity) {
    if (!queue) {
        return "Invalid parameters (queue)";
    }

    if (!queue->items){
        return "Memory allocation failed";
    }

    queue->capacity = capacity;
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->finished = 0;

    if (pthread_mutex_init(&queue->lock, NULL) != 0) {
        return "Mutex init failed";
    }
    if (monitor_init(&queue->not_full_monitor) != 0 ||
        monitor_init(&queue->not_empty_monitor) != 0 ||
        monitor_init(&queue->finished_monitor) != 0) {
        return "Monitor init failed";
    }
    return NULL;
}

void consumer_producer_destroy(consumer_producer_t* queue) {
    if (!queue) {
        return;
    }

    pthread_mutex_destroy(&queue->lock);
    free(queue->items);
    monitor_destroy(&queue->not_full_monitor);
    monitor_destroy(&queue->not_empty_monitor);
    monitor_destroy(&queue->finished_monitor);
}

const char* consumer_producer_put(consumer_producer_t* queue, const char* item) {

}

char* consumer_producer_get(consumer_producer_t* queue);

void consumer_producer_signal_finished(consumer_producer_t* queue) {
    if (!queue) {
        return;
    }

    pthread_mutex_lock(&queue->lock);
    queue->finished = 1;
    pthread_mutex_unlock(&queue->lock);

    monitor_signal(&queue->not_empty_monitor);
    monitor_signal(&queue->finished_monitor);
}

int consumer_producer_wait_finished(consumer_producer_t* queue) {
    if (!queue) {
        return -1;
    }
    return monitor_wait(&queue->finished_monitor);
}
