#define BUFFER_SIZE 1025
#define MAX_INT 2147483647
#define END "<END>"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include "plugins/plugin_sdk.h"

// typedefs for plugin interface
typedef const char* (*plugin_get_name_func_t)(void);
typedef const char* (*plugin_init_func_t)(int queue_size);
typedef const char* (*plugin_fini_func_t)(void);
typedef const char* (*plugin_place_work_func_t)(const char* str);
typedef void (*plugin_attach_func_t)(const char* (*next_place_work)(const char*));
typedef const char* (*plugin_wait_finished_func_t)(void);
typedef void (*set_shared_output_mutex_func_t)(pthread_mutex_t* mutex);

typedef struct {
    plugin_get_name_func_t get_name;
    plugin_init_func_t init;
    plugin_fini_func_t fini;
    plugin_place_work_func_t place_work;
    plugin_attach_func_t attach;
    plugin_wait_finished_func_t wait_finished;
    char* name;
    void* handle;
} plugin_handle_t;

// global output synchronization
static pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_usage() {
    printf("Usage: ./analyzer <queue_size> <plugin1> <plugin2> ... <pluginN>\n"
           "\n"
           "Arguments:\n"
           "  queue_size     Maximum number of items in each plugin's queue\n"
           "  plugin1..N     Names of plugins to load (without .so extension)\n"
           "\n"
           "Available plugins:\n"
           "  logger         - Logs all strings that pass through\n"
           "  typewriter     - Simulates typewriter effect with delays\n"
           "  uppercaser     - Converts strings to uppercase\n"
           "  rotator        - Move every character to the right. Last character moves to\n"
           "                   the beginning.\n"
           "  flipper        - Reverses the order of characters\n"
           "  expander       - Expands each character with spaces\n"
           "\n"
           "Example:\n"
           "  ./analyzer 20 uppercaser rotator logger\n"
           "  echo 'hello' | ./analyzer 20 uppercaser rotator logger\n"
           "  echo '<END>' | ./analyzer 20 uppercaser rotator logger\n");
}

int is_valid_int(const char* str) {
    if (!str || *str == '\0') {
        return 0;
    }
    
    while (*str == ' ' || *str == '\t') {
         str++;
    }

    if (!isdigit(*str)) {
        return 0;
    }
    
    while (*str) {
        if (!isdigit(*str)){
            return 0;
        }
        str++;
    }
    return 1;
}

int validate_args(int argc, char* argv[], int* queue_size) {
    if (argc < 3) {
        fprintf(stderr, "Error: Invalid argument count\n");
        print_usage();
        return 1;
    }
    
    if(!is_valid_int(argv[1])) {
        fprintf(stderr, "Error: queue_size must be a positive integer.\n");
        print_usage();
        return 1;
    }
    
    *queue_size = atoi(argv[1]);
    return 0;
}

int load_plugins(char* argv[], plugin_handle_t* plugins, int num_plugins) {
     // initialize all plugin handles to zero
    memset(plugins, 0, num_plugins * sizeof(plugin_handle_t)); 
    
    for (int i = 0; i < num_plugins; i++) {
        char* plugin_name = argv[i + 2];
        
        char filename[256];
        snprintf(filename, sizeof(filename), "./output/%s.so", plugin_name);
        
        void* handle = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
        if (!handle) {
            fprintf(stderr, "Error: Failed to load plugin [%s]: %s\n", plugin_name, dlerror());
            print_usage();
            // clean loaded plugins
            for (int j = 0; j < i; j++) {
                dlclose(plugins[j].handle);
            }
            return 1;
        }
        
        // clear existing error
        dlerror();
        
        plugin_get_name_func_t get_name = (plugin_get_name_func_t)dlsym(handle, "plugin_get_name");
        plugin_init_func_t init = (plugin_init_func_t)dlsym(handle, "plugin_init");
        plugin_fini_func_t fini = (plugin_fini_func_t)dlsym(handle, "plugin_fini");
        plugin_place_work_func_t place_work = (plugin_place_work_func_t)dlsym(handle, "plugin_place_work");
        plugin_attach_func_t attach = (plugin_attach_func_t)dlsym(handle, "plugin_attach");
        plugin_wait_finished_func_t wait_finished = (plugin_wait_finished_func_t)dlsym(handle, "plugin_wait_finished");
        set_shared_output_mutex_func_t set_mutex = (set_shared_output_mutex_func_t)dlsym(handle, "set_shared_output_mutex");
        
        // handle dlsym (exporting) errors
        char* error = dlerror();
        if (error != NULL || !get_name || !init || !fini || !place_work || !attach || !wait_finished) {
            fprintf(stderr, "Error: Plugin [%s] missing required functions: %s\n", plugin_name, error ? error : "Unknown symbol error");
            print_usage();
            dlclose(handle);
            // clean loaded plugins
            for (int j = 0; j < i; j++) {
                dlclose(plugins[j].handle);
            }
            return 1;
        }
        
        // set shared output mutex (if supported)
        if (set_mutex) {
            set_mutex(&output_mutex);
        }
        
        // store current plugin info
        plugins[i].get_name = get_name;
        plugins[i].init = init;
        plugins[i].fini = fini;
        plugins[i].place_work = place_work;
        plugins[i].attach = attach;
        plugins[i].wait_finished = wait_finished;
        plugins[i].name = plugin_name;
        plugins[i].handle = handle;
    }
    
    return 0;
}

int initialize_plugins(plugin_handle_t* plugins, int num_plugins, int queue_size) {
    for (int i = 0; i < num_plugins; i++) {
        const char* error = plugins[i].init(queue_size);
        if (error != NULL) {
            fprintf(stderr, "Error: Plugin [%s] failed to initialize: %s\n", plugins[i].name, error);

            // clean (finalize) previous plugins
            for (int j = 0; j < i; j++) {
                plugins[j].fini();
            }
            return 2;
        }
    }
    return 0;
}

int attach_plugins(plugin_handle_t* plugins, int num_plugins) {
    for (int i = 0; i < num_plugins - 1; i++) {
        plugins[i].attach(plugins[i+1].place_work);
    }
    return 0;
}

void cleanup_plugins(plugin_handle_t* plugins, int num_plugins) {
    for (int i = 0; i < num_plugins; i++) {
        if (plugins[i].handle) {
            plugins[i].fini();
            dlclose(plugins[i].handle);
        }
    }
}

int process_input(plugin_handle_t* plugins) {
    char buffer[BUFFER_SIZE];
    
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // replace trailing newline with null
            int len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            
            // sends item to first plugin
            const char* error = plugins[0].place_work(buffer);
            if (error != NULL) {
                fprintf(stderr, "Error: Failed to place work in first plugin: %s\n", error);
                return 1;
            }

            if (strcmp(buffer, END) == 0) {
                break;
            }
        } else {
            // EOF => busy wait forever.......
        }
    }
    
    return 0;
}

int wait_for_plugins(plugin_handle_t* plugins, int num_plugins) {
    for (int i = 0; i < num_plugins; i++) {
        const char* error = plugins[i].wait_finished();
        if (error != NULL) {
            fprintf(stderr, "Error: Plugin [%s] failed to finish gracefully: %s\n", plugins[i].name, error);
            return 1;
        }
    }

    return 0;
}



int main(int argc, char* argv[]) {
    // parse and validate args:
    int queue_size;
    if (validate_args(argc, argv, &queue_size) != 0) {
        return 1;
    }

    // allocate handles for plugins
    int num_plugins = argc - 2;
    plugin_handle_t* plugins = malloc(num_plugins * sizeof(plugin_handle_t));
    if (!plugins) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }
    
    // load plugin shared objects
    if (load_plugins(argv, plugins, num_plugins) != 0) {
        free(plugins);
        return 1;
    }
    
    // initialize plugins
    if (initialize_plugins(plugins, num_plugins, queue_size) != 0) {
        cleanup_plugins(plugins, num_plugins);
        free(plugins);
        return 2;
    }
    
    // attach plugins together
    if (attach_plugins(plugins, num_plugins) != 0) {
        cleanup_plugins(plugins, num_plugins);
        free(plugins);
        return 2;
    }
    
    // read Input from STDIN
    if (process_input(plugins) != 0) {
        cleanup_plugins(plugins, num_plugins);
        free(plugins);
        return 1;
    }

    // wait for plugins to finish
    if (wait_for_plugins(plugins, num_plugins) != 0) {
        cleanup_plugins(plugins, num_plugins);
        free(plugins);
        return 1;
    }
    
    // cleanup
    cleanup_plugins(plugins, num_plugins);
    free(plugins);
    printf("Pipeline shutdown complete\n");
    return 0;
}
