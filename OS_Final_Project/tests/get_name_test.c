#include <stdio.h>
#include "../plugins/plugin_common.h"

int main() {
    printf("Testing plugin_get_name function:\n");
    
    // Test with no name set (should return NULL)
    const char* name = plugin_get_name();
    printf("Name when uninitialized: %s\n", name ? name : "NULL");
    
    // We can't directly test setting the name since g_plugin_context is static
    // This would normally be set by common_plugin_init()
    printf("Note: Name would be set by common_plugin_init() function\n");
    
    printf("plugin_get_name test completed\n");
    return 0;
}
