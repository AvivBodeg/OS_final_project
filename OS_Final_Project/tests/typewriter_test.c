#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../plugins/typewriter.c"

void test_typewriter_transform() {
    printf("Testing typewriter transformation:\n");
    
    // Test basic string
    const char* input1 = "hello";
    const char* result1 = plugin_transform(input1);
    printf("Input: '%s'\n", input1);
    printf("Expected output: hello (with typewriter effect)\n");
    assert(result1 != NULL);
    assert(strcmp(result1, input1) == 0);
    printf("✓ Basic typewriter test passed\n\n");
    free((void*)result1);
    
    // Test with spaces and punctuation
    const char* input2 = "Hi there!";
    const char* result2 = plugin_transform(input2);
    printf("Input: '%s'\n", input2);
    printf("Expected output: Hi there! (with typewriter effect)\n");
    assert(result2 != NULL);
    assert(strcmp(result2, input2) == 0);
    printf("✓ Mixed content test passed\n\n");
    free((void*)result2);
    
    // Test empty string
    const char* input3 = "";
    const char* result3 = plugin_transform(input3);
    printf("Input: '' (empty string)\n");
    assert(result3 != NULL);
    assert(strcmp(result3, "") == 0);
    printf("✓ Empty string test passed\n\n");
    free((void*)result3);
    
    // Test NULL input
    const char* result4 = plugin_transform(NULL);
    assert(result4 == NULL);
    printf("✓ NULL input correctly handled\n");
    
    printf("All typewriter transformation tests passed!\n");
}

int main() {
    test_typewriter_transform();
    return 0;
}
