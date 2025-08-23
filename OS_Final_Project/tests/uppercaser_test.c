#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the plugin (we'll declare the function manually for testing)
extern const char* plugin_transform(const char* input);

int main() {
    printf("Testing uppercaser transformation:\n");
    
    // Test 1: Normal string
    const char* result1 = plugin_transform("hello world");
    if (result1) {
        printf("Input: 'hello world' -> Output: '%s'\n", result1);
        if (strcmp(result1, "HELLO WORLD") == 0) {
            printf("✓ Basic uppercase test passed\n");
        } else {
            printf("✗ Basic uppercase test failed\n");
            return 1;
        }
        free((void*)result1);
    } else {
        printf("✗ Transform returned NULL for valid input\n");
        return 1;
    }
    
    // Test 2: Mixed case with numbers and symbols
    const char* result2 = plugin_transform("Hello123!@#world");
    if (result2) {
        printf("Input: 'Hello123!@#world' -> Output: '%s'\n", result2);
        if (strcmp(result2, "HELLO123!@#WORLD") == 0) {
            printf("✓ Mixed case test passed\n");
        } else {
            printf("✗ Mixed case test failed\n");
            return 1;
        }
        free((void*)result2);
    } else {
        printf("✗ Transform returned NULL for mixed input\n");
        return 1;
    }
    
    // Test 3: Empty string
    const char* result3 = plugin_transform("");
    if (result3) {
        printf("Input: '' -> Output: '%s'\n", result3);
        if (strcmp(result3, "") == 0) {
            printf("✓ Empty string test passed\n");
        } else {
            printf("✗ Empty string test failed\n");
            return 1;
        }
        free((void*)result3);
    } else {
        printf("✗ Transform returned NULL for empty string\n");
        return 1;
    }
    
    // Test 4: NULL input
    const char* result4 = plugin_transform(NULL);
    if (result4 == NULL) {
        printf("✓ NULL input correctly handled\n");
    } else {
        printf("✗ NULL input should return NULL\n");
        return 1;
    }
    
    printf("All uppercaser transformation tests passed!\n");
    return 0;
}
