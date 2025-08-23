#define PLUGIN_NAME "typewriter"
#define DELAY 100000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
    
    printf("[%s] ", PLUGIN_NAME);
    for (int i = 0; i < len; i++) {
        printf("%c", input[i]);
        fflush(stdout);
        usleep(DELAY);
    }
    printf("\n");
    
    strcpy(result, input);
    return result;
}

const char* plugin_init(int queue_size) {
    return common_plugin_init(plugin_transform, PLUGIN_NAME, queue_size);
}
