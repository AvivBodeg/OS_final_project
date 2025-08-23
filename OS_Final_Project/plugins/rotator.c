#define PLUGIN_NAME "rotator"

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

    if (len > 0) {
        for (int i = 1; i < len; i++) {
            result[i] = input[i-1];
        }
        result[0] = input[len-1];
    } else {
        result[0] = input[0];
    }
    result[len] = '\0';

    return result;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, PLUGIN_NAME, queue_size);
}
