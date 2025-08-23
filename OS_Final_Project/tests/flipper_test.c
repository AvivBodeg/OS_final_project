#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../plugins/flipper.c"

void test_flipper_transform() {
    printf("Testing flipper transformation:\n");
    
    // Test basic string reversal
    const char* input1 = "hello";
    const char* result1 = plugin_transform(input1);
    printf("Input: '%s' -> Output: '%s'\n", input1, result1);
    printf("Expected: 'olleh'\n");
    assert(result1 != NULL);
    assert(strcmp(result1, "olleh") == 0);
    printf("✓ Basic flip test passed\n\n");
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
    
    // Test palindrome
    const char* input3 = "racecar";
    const char* result3 = plugin_transform(input3);
    printf("Input: '%s' -> Output: '%s'\n", input3, result3);
    printf("Expected: 'racecar' (palindrome)\n");
    assert(result3 != NULL);
    assert(strcmp(result3, "racecar") == 0);
    printf("✓ Palindrome test passed\n\n");
    free((void*)result3);
    
    // Test with numbers and symbols
    const char* input4 = "abc123!";
    const char* result4 = plugin_transform(input4);
    printf("Input: '%s' -> Output: '%s'\n", input4, result4);
    printf("Expected: '!321cba'\n");
    assert(result4 != NULL);
    assert(strcmp(result4, "!321cba") == 0);
    printf("✓ Mixed content test passed\n\n");
    free((void*)result4);
    
    // Test empty string
    const char* input5 = "";
    const char* result5 = plugin_transform(input5);
    printf("Input: '' -> Output: '%s'\n", result5);
    assert(result5 != NULL);
    assert(strcmp(result5, "") == 0);
    printf("✓ Empty string test passed\n\n");
    free((void*)result5);
    
    // Test NULL input
    const char* result6 = plugin_transform(NULL);
    assert(result6 == NULL);
    printf("✓ NULL input correctly handled\n");
    
    printf("All flipper transformation tests passed!\n");
}

int main() {
    test_flipper_transform();
    return 0;
}
