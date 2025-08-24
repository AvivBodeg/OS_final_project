#define PLUGIN_NAME "logger"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plugin_common.h"

const char* plugin_transform(const char* input) {
    if (!input) {
        return NULL;
    }
    
    int len = strlen(input);
    char* result = malloc(len + 1);
    if (!result) {
        return NULL;
    }
    
    lock_output();
    printf("[%s] %s\n", PLUGIN_NAME, input);
    unlock_output();
    
    strcpy(result, input);
    return result;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, PLUGIN_NAME, queue_size);
}
