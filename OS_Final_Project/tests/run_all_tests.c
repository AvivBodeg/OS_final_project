#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    const char* name;
    const char* executable;
    int passed;
} TestResult;

int run_test(const char* test_name, const char* executable) {
    printf("Running %s...\n", test_name);
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process - run the test
        execl(executable, executable, NULL);
        exit(1); // If execl fails
    } else if (pid > 0) {
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("✓ %s PASSED\n\n", test_name);
            return 1;
        } else {
            printf("✗ %s FAILED\n\n", test_name);
            return 0;
        }
    } else {
        printf("✗ %s FAILED (fork error)\n\n", test_name);
        return 0;
    }
}

int main() {
    printf("=== PLUGIN TEST SUITE ===\n");
    printf("Running all plugin tests...\n\n");
    
    TestResult tests[] = {
        {"Uppercaser Plugin", "./output/uppercaser_test", 0},
        {"Logger Plugin", "./output/logger_test", 0},
        {"Typewriter Plugin", "./output/typewriter_test", 0},
        {"Rotator Plugin", "./output/rotator_test", 0},
        {"Flipper Plugin", "./output/flipper_test", 0},
        {"Expander Plugin", "./output/expander_test", 0}
    };
    
    int total_tests = sizeof(tests) / sizeof(TestResult);
    int passed_tests = 0;
    
    // Run each test
    for (int i = 0; i < total_tests; i++) {
        tests[i].passed = run_test(tests[i].name, tests[i].executable);
        passed_tests += tests[i].passed;
    }
    
    // Print summary
    printf("=== TEST RESULTS SUMMARY ===\n");
    for (int i = 0; i < total_tests; i++) {
        printf("%s: %s\n", tests[i].name, tests[i].passed ? "PASSED" : "FAILED");
    }
    
    printf("\n=== FINAL RESULTS ===\n");
    printf("Tests passed: %d/%d\n", passed_tests, total_tests);
    
    double success_rate = (double)passed_tests / total_tests * 100.0;
    printf("Success rate: %.1f%%\n", success_rate);
    
    if (success_rate == 100.0) {
        printf("🎉 ALL TESTS PASSED! 🎉\n");
    } else if (success_rate >= 80.0) {
        printf("⚠️  Most tests passed, but some issues remain.\n");
    } else {
        printf("❌ Multiple test failures detected.\n");
    }
    
    return (success_rate == 100.0) ? 0 : 1;
}
