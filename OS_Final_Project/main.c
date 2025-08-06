#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error: Invalid arguments.\n");
        printf("Usage: ./analyzer <queue_size> <plugin1> <plugin2> ... <pluginN>\n");
        return 1;
    }

    int queue_size = atoi(argv[1]);
    if (queue_size <= 0) {
        fprintf(stderr, "Error: queue_size must be a positive integer.\n");
        return 1;
    }

    printf("Queue size: %d\n", queue_size);
    for (int i = 2; i < argc; ++i) {
        printf("Plugin %d: %s\n", i - 1, argv[i]);
    }

    return 0;
}
