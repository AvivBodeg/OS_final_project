#include <stdio.h>
#include "../plugins/sync/consumer_producer.h"

int main() {
    consumer_producer_t queue;
    
    // Test capacity 0 - should fail
    printf("Testing capacity 0...\n");
    const char* result = consumer_producer_init(&queue, 0);
    if (result) {
        printf("✓ Capacity 0 correctly rejected: %s\n", result);
    } else {
        printf("✗ Capacity 0 was incorrectly accepted!\n");
        consumer_producer_destroy(&queue);
        return 1;
    }
    
    // Test negative capacity - should fail
    printf("Testing capacity -1...\n");
    result = consumer_producer_init(&queue, -1);
    if (result) {
        printf("✓ Negative capacity correctly rejected: %s\n", result);
    } else {
        printf("✗ Negative capacity was incorrectly accepted!\n");
        consumer_producer_destroy(&queue);
        return 1;
    }
    
    // Test valid capacity - should succeed
    printf("Testing capacity 1...\n");
    result = consumer_producer_init(&queue, 1);
    if (!result) {
        printf("✓ Capacity 1 correctly accepted\n");
        consumer_producer_destroy(&queue);
    } else {
        printf("✗ Capacity 1 was incorrectly rejected: %s\n", result);
        return 1;
    }
    
    printf("All capacity validation tests passed!\n");
    return 0;
}
