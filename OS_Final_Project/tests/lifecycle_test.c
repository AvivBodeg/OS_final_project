#include <stdio.h>
#include <unistd.h>
#include "../plugins/plugin_common.h"

// Mock process function
const char* test_process(const char* input) {
    printf("Processing: %s\n", input);
    return input; // Return unchanged
}

int main() {
    printf("Testing complete plugin lifecycle:\n");
    
    // Test 1: Initialize plugin
    const char* result = common_plugin_init(test_process, "TestPlugin", 3);
    if (!result) {
        printf("✓ Plugin initialized successfully\n");
    } else {
        printf("✗ Plugin initialization failed: %s\n", result);
        return 1;
    }
    
    // Test 2: Place some work
    result = plugin_place_work("hello");
    if (!result) {
        printf("✓ Work placed successfully\n");
    } else {
        printf("✗ Failed to place work: %s\n", result);
        return 1;
    }
    
    // Test 3: Place end signal
    result = plugin_place_work("<END>");
    if (!result) {
        printf("✓ End signal placed successfully\n");
    } else {
        printf("✗ Failed to place end signal: %s\n", result);
        return 1;
    }
    
    // Give the thread a moment to process
    usleep(100000); // 100ms
    
    // Test 4: Wait for completion
    result = plugin_wait_finished();
    if (!result) {
        printf("✓ Plugin wait finished successfully\n");
    } else {
        printf("✗ Failed to wait for finish: %s\n", result);
        return 1;
    }
    
    // Test 5: Finalize plugin
    result = plugin_fini();
    if (!result) {
        printf("✓ Plugin finalized successfully\n");
    } else {
        printf("✗ Failed to finalize plugin: %s\n", result);
        return 1;
    }
    
    // Test 6: Try to use after finalization (should fail)
    result = plugin_place_work("test");
    if (result) {
        printf("✓ Work correctly rejected after finalization: %s\n", result);
    } else {
        printf("✗ Work should have been rejected after finalization\n");
        return 1;
    }
    
    printf("All lifecycle tests passed!\n");
    return 0;
}
