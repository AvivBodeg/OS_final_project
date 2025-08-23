#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../plugins/typewriter.c"

int main() {
    printf("Testing typewriter effect with a longer string:\n");
    printf("You should see each character appear one by one with delays...\n\n");
    
    const char* test_string = "This is a longer test string to see the typewriter effect!";
    const char* result = plugin_transform(test_string);
    
    printf("\nResult matches input: %s\n", strcmp(result, test_string) == 0 ? "YES" : "NO");
    free((void*)result);
    
    return 0;
}
