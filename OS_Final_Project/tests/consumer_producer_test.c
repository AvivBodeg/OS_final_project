#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../plugins/sync/consumer_producer.h"

void* producer_thread(void* arg) {
    consumer_producer_t* queue = (consumer_producer_t*)arg;
    
    const char* items[] = {"hello", "world", "test", "<END>"};
    for (int i = 0; i < 4; i++) {
        printf("Producing: %s\n", items[i]);
        const char* result = consumer_producer_put(queue, items[i]);
        if (result) {
            printf("Error producing: %s\n", result);
            break;
        }
        usleep(100000); // 100ms delay
    }
    
    consumer_producer_signal_finished(queue);
    return NULL;
}

void* consumer_thread(void* arg) {
    consumer_producer_t* queue = (consumer_producer_t*)arg;
    
    char* item;
    while ((item = consumer_producer_get(queue)) != NULL) {
        printf("Consumed: %s\n", item);
        if (strcmp(item, "<END>") == 0) {
            free(item);
            break;
        }
        free(item);
        usleep(150000); // 150ms delay
    }
    
    return NULL;
}

int main() {
    consumer_producer_t queue;
    
    // Test initialization
    const char* result = consumer_producer_init(&queue, 2);
    if (result) {
        printf("Failed to init queue: %s\n", result);
        return 1;
    }
    
    pthread_t producer, consumer;
    
    // Start threads
    pthread_create(&consumer, NULL, consumer_thread, &queue);
    pthread_create(&producer, NULL, producer_thread, &queue);
    
    // Wait for completion
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    
    // Wait for finished signal
    if (consumer_producer_wait_finished(&queue) == 0) {
        printf("Queue finished successfully\n");
    }
    
    // Cleanup
    consumer_producer_destroy(&queue);
    
    printf("Test completed\n");
    return 0;
}
