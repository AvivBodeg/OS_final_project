#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../plugins/logger.c"

void test_logger_transform() {
    printf("Testing logger transformation:\n");
    
    // Test basic logging
    const char* input1 = "hello world";
    printf("Expected output: [logger] hello world\n");
    const char* result1 = plugin_transform(input1);
    assert(result1 != NULL);
    assert(strcmp(result1, input1) == 0);
    printf("✓ Basic logging test passed\n\n");
    free((void*)result1);
    
    // Test with special characters
    const char* input2 = "Hello123!@#";
    printf("Expected output: [logger] Hello123!@#\n");
    const char* result2 = plugin_transform(input2);
    assert(result2 != NULL);
    assert(strcmp(result2, input2) == 0);
    printf("✓ Special characters test passed\n\n");
    free((void*)result2);
    
    // Test empty string
    const char* input3 = "";
    printf("Expected output: [logger] \n");
    const char* result3 = plugin_transform(input3);
    assert(result3 != NULL);
    assert(strcmp(result3, "") == 0);
    printf("✓ Empty string test passed\n\n");
    free((void*)result3);
    
    // Test NULL input
    const char* result4 = plugin_transform(NULL);
    assert(result4 == NULL);
    printf("✓ NULL input correctly handled\n");
    
    printf("All logger transformation tests passed!\n");
}

int main() {
    test_logger_transform();
    return 0;
}
