#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../plugins/rotator.c"

void test_rotator_transform() {
    printf("Testing rotator transformation:\n");
    
    // Test basic string rotation
    const char* input1 = "hello";
    const char* result1 = plugin_transform(input1);
    printf("Input: '%s' -> Output: '%s'\n", input1, result1);
    printf("Expected: 'ohell'\n");
    assert(result1 != NULL);
    assert(strcmp(result1, "ohell") == 0);
    printf("✓ Basic rotation test passed\n\n");
    free((void*)result1);
    
    // Test single character
    const char* input2 = "a";
    const char* result2 = plugin_transform(input2);
    printf("Input: '%s' -> Output: '%s'\n", input2, result2);
    printf("Expected: 'a' (single char unchanged)\n");
    assert(result2 != NULL);
    assert(strcmp(result2, "a") == 0);
    printf("✓ Single character test passed\n\n");
    free((void*)result2);
    
    // Test two characters
    const char* input3 = "ab";
    const char* result3 = plugin_transform(input3);
    printf("Input: '%s' -> Output: '%s'\n", input3, result3);
    printf("Expected: 'ba'\n");
    assert(result3 != NULL);
    assert(strcmp(result3, "ba") == 0);
    printf("✓ Two character test passed\n\n");
    free((void*)result3);
    
    // Test empty string
    const char* input4 = "";
    const char* result4 = plugin_transform(input4);
    printf("Input: '' -> Output: '%s'\n", result4);
    assert(result4 != NULL);
    assert(strcmp(result4, "") == 0);
    printf("✓ Empty string test passed\n\n");
    free((void*)result4);
    
    // Test NULL input
    const char* result5 = plugin_transform(NULL);
    assert(result5 == NULL);
    printf("✓ NULL input correctly handled\n");
    
    printf("All rotator transformation tests passed!\n");
}

int main() {
    test_rotator_transform();
    return 0;
}
