#include <stdio.h>
#include "../plugins/plugin_common.h"

int main() {
    plugin_context_t test_context = {0};
    test_context.name = "TestPlugin";
    
    printf("Testing log functions:\n");
    log_info(&test_context, "Plugin initialized successfully");
    log_error(&test_context, "Example error message");
    
    // Test null safety
    log_info(NULL, "This should not print");
    log_info(&test_context, NULL);
    
    plugin_context_t empty_context = {0};
    log_info(&empty_context, "This should not print either");
    
    printf("Log test completed\n");
    return 0;
}
