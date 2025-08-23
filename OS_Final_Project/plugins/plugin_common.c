#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "plugin_common.h"

static plugin_context_t plugin_context = {0}; // all integers are 0 and all pointers are NULL

void* plugin_consumer_thread(void* arg) {
    plugin_context_t* context = (plugin_context_t*)arg;

    if (!context) {
        return NULL;
    }

    while(1) {
        char* work_item = consumer_producer_get(context->queue);
        
        if (!work_item) {
            break;
        }

        // forward shutdown to next plugin
        if (strcmp(work_item, "<END>") == 0) {
            if (context->next_place_work) {
                context->next_place_work("<END>");
            }
            free(work_item);
            break;
        }

        // process and send to next plugin
        const char* processed_result = context->process_function(work_item);
        if (processed_result) {
            if (context->next_place_work) {
                const char* forward_result = context->next_place_work(processed_result);
                if (forward_result) {
                    log_error(context, "Failed to forward work to next plugin");
                }
            }
        } else {
            log_error(context, "Plugin transformation returned NULL");
        }
        free(work_item);
    }
    
    context->finished = 1;
    consumer_producer_signal_finished(context->queue);
    return NULL;
}

void log_error(plugin_context_t* context, const char* message) {
    if (context && context->name && message) {
        fprintf(stderr, "[ERROR][%s] - %s\n", context->name, message);
    }
}

void log_info(plugin_context_t* context, const char* message) {
    if (context && context->name && message) {
        printf("[INFO][%s] - %s\n", context->name, message);
    }
}

const char* common_plugin_init(const char* (*process_function)(const char*), const char* name, int queue_size) {
    if (!process_function) {
        return "Invalid process function";
    }
    if (!name || queue_size <= 0) {
        return "Invalid plugin parameters";
    }
    if (plugin_context.initialized) {
        return "Plugin already initialized";
    }

    plugin_context.process_function = process_function;
    plugin_context.name = name;
    
    plugin_context.queue = malloc(sizeof(consumer_producer_t));
    if (!plugin_context.queue) {
        return "Failed to allocate memory for queue";
    }
    const char* init_result = consumer_producer_init(plugin_context.queue, queue_size);
    if (init_result) {
        free(plugin_context.queue);
        plugin_context.queue = NULL;
        return init_result;
    }

    if (pthread_create(&plugin_context.consumer_thread, NULL, plugin_consumer_thread, &plugin_context) != 0) {
        consumer_producer_destroy(plugin_context.queue);
        free(plugin_context.queue);
        plugin_context.queue = NULL;
        return "Failed to create consumer thread";
    }

    plugin_context.initialized = 1;
    return NULL;
}

const char* plugin_fini(void) {
    if (!plugin_context.initialized) {
        return "Plugin not initialized";
    }
    
    consumer_producer_signal_finished(plugin_context.queue);
    
    if (pthread_join(plugin_context.consumer_thread, NULL) != 0) {
        return "Failed to join consumer thread";
    }
    
    // clean the queue
    consumer_producer_destroy(plugin_context.queue);
    free(plugin_context.queue);
    
    // reset the context
    plugin_context.queue = NULL;
    plugin_context.name = NULL;
    plugin_context.process_function = NULL;
    plugin_context.next_place_work = NULL;
    plugin_context.initialized = 0;
    plugin_context.finished = 0;
    
    return NULL;
}

const char* plugin_get_name(void) {
    return plugin_context.name;
}

const char* plugin_place_work(const char* str) {
    if (!plugin_context.initialized) {
        return "Plugin not initialized";
    }
    if (!str) {
        return "Invalid input string";
    }

    return consumer_producer_put(plugin_context.queue, str); //NULL if succeeded
}

void plugin_attach(const char* (*next_place_work)(const char*)) {
    plugin_context.next_place_work = next_place_work;
}

const char* plugin_wait_finished(void) {
    if (!plugin_context.initialized) {
        return "Plugin not initialized";
    }
    
    if (consumer_producer_wait_finished(plugin_context.queue) != 0) {
        return "Failed to wait for queue completion";
    }
    
    return NULL;
}
