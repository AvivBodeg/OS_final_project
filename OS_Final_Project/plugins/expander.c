#define PLUGIN_NAME "expander"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plugin_common.h"

const char* plugin_transform(const char* input) {
    if (!input) {
        return NULL;
    }
    
    int input_len = strlen(input);
    
    // empty string handling
    if (input_len == 0) {
        char* result = malloc(1);
        if (!result) {
            return NULL;
        }

        result[0] = '\0';
        return result;
    }
    
    int result_len = 2 * input_len - 1;
    char* result = malloc(result_len + 1);
    if (!result) {
        return NULL;
    }
    
    int result_index = 0;
    for (int i = 0; i < input_len; i++) {
        result[result_index++] = input[i];
        if (i < input_len - 1) {
            result[result_index++] = ' ';
        }
    }
    
    result[result_len] = '\0';
    return result;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, PLUGIN_NAME, queue_size);
}
