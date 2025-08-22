#ifndef PLUGIN_COMMON_C
#define PLUGIN_COMMON_C

#include "plugin_common.h"

void *plugin_consumer_thread(void *arg)
{
    return NULL;
}

void log_error(plugin_context_t *context, const char *message)
{
}

void log_info(plugin_context_t *context, const char *message)
{
}

const char *plugin_get_name(void)
{
    return NULL;
}

const char *common_plugin_init(const char *(*process_function)(const char *), const char *name, int queue_size)
{
    return NULL;
}

const char *plugin_init(int queue_size)
{
    return NULL;
}

const char *plugin_fini(void)
{
    return NULL;
}

const char *plugin_place_work(const char *str)
{
    return NULL;
}

void plugin_attach(const char *(*next_place_work)(const char *))
{
}

const char *plugin_wait_finished(void)
{
    return NULL;
}

#endif