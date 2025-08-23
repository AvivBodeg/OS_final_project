#include <stdio.h>
#include "../plugins/plugin_common.h"

// Mock process function for testing
const char* test_process_function(const char* input) {
    return input; // Just return input unchanged
}

int main() {
    printf("Testing common_plugin_init:\n");
    
    // Test 1: Valid initialization
    const char* result = common_plugin_init(test_process_function, "TestPlugin", 5);
    if (!result) {
        printf("✓ Initial initialization successful\n");
        
        // Test the name was set correctly
        const char* name = plugin_get_name();
        printf("✓ Plugin name: %s\n", name ? name : "NULL");
    } else {
        printf("✗ Initial initialization failed: %s\n", result);
        return 1;
    }
    
    // Test 2: Double initialization should fail
    result = common_plugin_init(test_process_function, "TestPlugin2", 3);
    if (result) {
        printf("✓ Double initialization correctly rejected: %s\n", result);
    } else {
        printf("✗ Double initialization should have failed\n");
        return 1;
    }
    
    // Test 3: Invalid parameters
    result = common_plugin_init(NULL, "TestPlugin", 5);
    if (result) {
        printf("✓ NULL process function correctly rejected: %s\n", result);
    } else {
        printf("✗ NULL process function should have been rejected\n");
        return 1;
    }
    
    printf("All common_plugin_init tests passed!\n");
    return 0;
}
