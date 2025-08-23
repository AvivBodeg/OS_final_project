#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../plugins/expander.c"

void test_expander_transform() {
    printf("Testing expander transformation:\n");
    
    // Test basic string expansion
    const char* input1 = "hello";
    const char* result1 = plugin_transform(input1);
    printf("Input: '%s' -> Output: '%s'\n", input1, result1 ? result1 : "NULL");
    printf("Expected: 'h e l l o'\n");
    assert(result1 != NULL);
    assert(strcmp(result1, "h e l l o") == 0);
    printf("✓ Basic expansion test passed\n\n");
    free((void*)result1);
    
    // Test single character
    const char* input2 = "a";
    const char* result2 = plugin_transform(input2);
    printf("Input: '%s' -> Output: '%s'\n", input2, result2 ? result2 : "NULL");
    printf("Expected: 'a' (single char unchanged)\n");
    assert(result2 != NULL);
    assert(strcmp(result2, "a") == 0);
    printf("✓ Single character test passed\n\n");
    free((void*)result2);
    
    // Test two characters
    const char* input3 = "hi";
    const char* result3 = plugin_transform(input3);
    printf("Input: '%s' -> Output: '%s'\n", input3, result3 ? result3 : "NULL");
    printf("Expected: 'h i'\n");
    assert(result3 != NULL);
    assert(strcmp(result3, "h i") == 0);
    printf("✓ Two character test passed\n\n");
    free((void*)result3);
    
    // Test with numbers and symbols
    const char* input4 = "abc123!";
    const char* result4 = plugin_transform(input4);
    printf("Input: '%s' -> Output: '%s'\n", input4, result4 ? result4 : "NULL");
    printf("Expected: 'a b c 1 2 3 !'\n");
    assert(result4 != NULL);
    assert(strcmp(result4, "a b c 1 2 3 !") == 0);
    printf("✓ Mixed content test passed\n\n");
    free((void*)result4);
    
    // Test string that already has spaces
    const char* input5 = "a b";
    const char* result5 = plugin_transform(input5);
    printf("Input: '%s' -> Output: '%s'\n", input5, result5 ? result5 : "NULL");
    printf("Expected: 'a   b' (space becomes space-space-space)\n");
    assert(result5 != NULL);
    assert(strcmp(result5, "a   b") == 0);
    printf("✓ Existing spaces test passed\n\n");
    free((void*)result5);
    
    // Test empty string
    const char* input6 = "";
    const char* result6 = plugin_transform(input6);
    printf("Input: '' -> Output: '%s'\n", result6 ? result6 : "NULL");
    assert(result6 != NULL);
    assert(strcmp(result6, "") == 0);
    printf("✓ Empty string test passed\n\n");
    free((void*)result6);
    
    // Test NULL input
    const char* result7 = plugin_transform(NULL);
    assert(result7 == NULL);
    printf("✓ NULL input correctly handled\n");
    
    printf("All expander transformation tests passed!\n");
}

int main() {
    test_expander_transform();
    return 0;
}
