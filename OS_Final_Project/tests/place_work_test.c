#include <stdio.h>
#include "../plugins/plugin_common.h"

// Mock process function
const char* test_process(const char* input) {
    return input; // Just return unchanged
}

// Mock next plugin function
const char* mock_next_plugin(const char* str) {
    printf("Mock next plugin received: %s\n", str);
    return NULL; // Success
}

int main() {
    printf("Testing plugin_place_work and plugin_attach:\n");
    
    // Test 1: place_work before initialization (should fail)
    const char* result = plugin_place_work("test");
    if (result) {
        printf("✓ place_work before init correctly rejected: %s\n", result);
    } else {
        printf("✗ place_work before init should have failed\n");
        return 1;
    }
    
    // Test 2: Initialize plugin
    result = common_plugin_init(test_process, "TestPlugin", 3);
    if (!result) {
        printf("✓ Plugin initialized successfully\n");
    } else {
        printf("✗ Plugin initialization failed: %s\n", result);
        return 1;
    }
    
    // Test 3: Test plugin_attach
    plugin_attach(mock_next_plugin);
    printf("✓ Plugin attach completed (no return value to check)\n");
    
    // Test 4: Test place_work after initialization
    result = plugin_place_work("hello");
    if (!result) {
        printf("✓ place_work after init successful\n");
    } else {
        printf("✗ place_work after init failed: %s\n", result);
        return 1;
    }
    
    // Test 5: Test null string
    result = plugin_place_work(NULL);
    if (result) {
        printf("✓ place_work with NULL correctly rejected: %s\n", result);
    } else {
        printf("✗ place_work with NULL should have failed\n");
        return 1;
    }
    
    printf("All place_work and attach tests passed!\n");
    return 0;
}
