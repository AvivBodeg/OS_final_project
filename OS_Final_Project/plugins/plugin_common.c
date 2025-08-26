#define END "<END>"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "plugin_common.h"

static plugin_context_t plugin_context = {0}; // all integers are 0 and all pointers are NULL
static pthread_mutex_t* shared_output_lock = NULL; // shared mutex for output sync

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

        // in case of "<END>": forward shutdown to next plugin withgout modifying
        if (strcmp(work_item, END) == 0) {
            if (context->next_place_work) {
                context->next_place_work(END);
            }
            free(work_item);
            break;
        }

        // process work and send to next plugin
        const char* result = context->process_function(work_item);
        if (result) {
            if (context->next_place_work) {
                const char* error_msg = context->next_place_work(result);
                if (error_msg) {
                    log_error(context, "Failed to forward string to next plugin");
                }
            }
            free((void*)result);
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

/***commented out for submission - left for future debugging (not like anyone will actually do that but...)
void log_info(plugin_context_t* context, const char* message) {
    if (context && context->name && message) {
        printf("[INFO][%s] - %s\n", context->name, message);
    }
}
*/

const char* common_plugin_init(const char* (*process_function)(const char*), const char* name, int queue_size) {
    if (!process_function) {
        return "Invalid process_function";
    }
    if ( queue_size <= 0 || !name) {
        return "Invalid plugin parameters";
    }
    if (plugin_context.initialized) {
        return "Plugin already up";
    }

    plugin_context.queue = malloc(sizeof(consumer_producer_t));
    if (!plugin_context.queue) {
        return "Failed to allocate memory for queue";
    }
    
    plugin_context.process_function = process_function;
    plugin_context.name = name;
    
    const char* init_error_msg = consumer_producer_init(plugin_context.queue, queue_size);
    if (init_error_msg) {
        free(plugin_context.queue);
        plugin_context.queue = NULL;
        return init_error_msg;
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
        return "Cannot process NULL";
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

__attribute__((visibility("default")))
void set_shared_output_mutex(pthread_mutex_t* lock) {
    shared_output_lock = lock;
}

void lock_output(void) {
    if (shared_output_lock != NULL) {
        pthread_mutex_lock(shared_output_lock);
    }
}

void unlock_output(void) {
    if (shared_output_lock != NULL) {
        pthread_mutex_unlock(shared_output_lock);
    }
}
