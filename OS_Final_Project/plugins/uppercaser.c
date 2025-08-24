#define PLUGIN_NAME "uppercaser"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    
    for (int i = 0; i < len; i++) {
        result[i] = toupper(input[i]);
    }
    
    result[len] = '\0';
    return result;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, PLUGIN_NAME, queue_size);
}
