#include <stdlib.h>
#include <string.h>
#include "consumer_producer.h"

const char* consumer_producer_init(consumer_producer_t* queue, int capacity) {
    if (!queue || capacity <= 0) {
        return "Invalid parameters: queue cannot be NULL and capacity must be positive";
    }

    queue->items = calloc(capacity, sizeof(char*));
    if (!queue->items) {
        return "Memory allocation failed";
    }

    queue->capacity = capacity;
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->finished = 0;

    if (pthread_mutex_init(&queue->lock, NULL) != 0) {
        free(queue->items);
        return "Mutex init failed";
    }
    if (monitor_init(&queue->not_full_monitor) != 0 ||
        monitor_init(&queue->not_empty_monitor) != 0 ||
        monitor_init(&queue->finished_monitor) != 0) {
        pthread_mutex_destroy(&queue->lock);
        free(queue->items);
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
    queue->items = NULL;
    monitor_destroy(&queue->not_full_monitor);
    monitor_destroy(&queue->not_empty_monitor);
    monitor_destroy(&queue->finished_monitor);
}

const char* consumer_producer_put(consumer_producer_t* queue, const char* item) {
    if (!queue || !item) {
        return "Invalid args";
    }

    pthread_mutex_lock(&queue->lock);

    // wait while the queue is full
    while (queue->count == queue->capacity && !queue->finished) {
        monitor_reset(&queue->not_full_monitor);
        pthread_mutex_unlock(&queue->lock);
        if (monitor_wait(&queue->not_full_monitor) != 0) {
            return "Failed in wait on not_full_monitor";
        }

        pthread_mutex_lock(&queue->lock);
    }

    if (queue->finished) {
        pthread_mutex_unlock(&queue->lock);
        return "Tried to put new item to a finished queue";
    }

    char* item_copy = strdup(item);
    if (!item_copy) {
        pthread_mutex_unlock(&queue->lock);
        return "Failed copying item in put";
    }

    queue->items[queue->tail] = item_copy;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->count++;

    if(queue->count == 1) {
        monitor_signal(&queue->not_empty_monitor);
    }

    pthread_mutex_unlock(&queue->lock);
    return NULL;
}

char* consumer_producer_get(consumer_producer_t* queue) {
    if (!queue) {
        return NULL;
    }

    pthread_mutex_lock(&queue->lock);

    //wait while the queue is empty
    while (queue->count == 0 && !queue->finished) {
        monitor_reset(&queue->not_empty_monitor);
        pthread_mutex_unlock(&queue->lock);
        if (monitor_wait(&queue->not_empty_monitor) != 0) {
            return NULL;
        }

        pthread_mutex_lock(&queue->lock);
    }

    if (queue->finished && queue->count == 0) {
        pthread_mutex_unlock(&queue->lock);
        return NULL;
    }

    char* item = queue->items[queue->head];
    queue->items[queue->head] = NULL;
    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;

    if (queue->count == queue->capacity - 1) {
        monitor_signal(&queue->not_full_monitor);
    }

    pthread_mutex_unlock(&queue->lock);
    return item;
}

void consumer_producer_signal_finished(consumer_producer_t* queue) {
    if (!queue) {
        return;
    }

    pthread_mutex_lock(&queue->lock);
    
    queue->finished = 1;
    monitor_signal(&queue->not_empty_monitor);
    monitor_signal(&queue->finished_monitor);
    monitor_signal(&queue->not_full_monitor);

    pthread_mutex_unlock(&queue->lock);
}

int consumer_producer_wait_finished(consumer_producer_t* queue) {
    if (!queue) {
        return -1;
    }
    return monitor_wait(&queue->finished_monitor);
}
