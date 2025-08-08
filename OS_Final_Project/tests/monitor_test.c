#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../plugins/sync/monitor.h"

typedef struct {
    monitor_t* m;
    int result;
} wait_arg_t;

void* waiter_thread(void* arg) {
    wait_arg_t* a = arg;
    a->result = monitor_wait(a->m);
    return NULL;
}

static int test_wait_then_signal(void) {
    monitor_t m;
    if (monitor_init(&m) != 0) return -1;
    wait_arg_t arg = { .m = &m, .result = -1 };
    pthread_t t;
    pthread_create(&t, NULL, waiter_thread, &arg);
    usleep(100 * 1000);
    monitor_signal(&m);
    pthread_join(t, NULL);
    int ok = (arg.result == 0);
    monitor_destroy(&m);
    return ok ? 0 : -1;
}

static int test_signal_before_wait(void) {
    monitor_t m;
    if (monitor_init(&m) != 0) return -1;
    monitor_signal(&m);
    wait_arg_t arg = { .m = &m, .result = -1 };
    pthread_t t;
    pthread_create(&t, NULL, waiter_thread, &arg);
    pthread_join(t, NULL);
    int ok = (arg.result == 0);
    monitor_destroy(&m);
    return ok ? 0 : -1;
}

int main(void) {
    int pass = 1;
    pass &= (test_wait_then_signal() == 0);
    pass &= (test_signal_before_wait() == 0);
    printf(pass ? "All tests passed!\n" : "Some tests failed!\n");
    return pass ? 0 : 1;
}

/**
 * bash
 * gcc -Iplugins -Wall -Wextra -o output/monitor_test plugins/sync/monitor.c tests/monitor_test.c -lpthread
 * ./output/monitor_test
**/