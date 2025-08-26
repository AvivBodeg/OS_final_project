#!/bin/bash

# colours
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NO_COLOUR='\033[0m'

TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0


# printing functions
print_pass() {
    echo -e "$1: ${GREEN}PASS${NO_COLOUR}"
    ((TESTS_PASSED++))
}

print_fail() {
    echo -e "$1: ${RED}FAIL${NO_COLOUR}"
    ((TESTS_FAILED++))
}

print_info() {
    echo -e "${BLUE}[INFO]${NO_COLOUR}: $1"
}

increment_test_count() {
    ((TESTS_TOTAL++))
}

# cleanup in case of 100% pass
cleanup_test_files() {
    rm -f output/test_output.txt
    rm -f output/valgrind_output.txt
}

# build
print_info "Building project..."
if ! ./build.sh > /dev/null 2>&1; then
    echo -e "${RED}[ERROR]${NO_COLOUR} Build failed"
    exit 1
fi
print_info "Build succeeded"

> output/test_output.txt

# input validation tests
print_info "Running validation tests"

increment_test_count;
if echo -e "test\n<END>" | ./output/analyzer aviv:')' logger >/dev/null 2>&1; then
    print_fail "Non-numeric queue size"
else
    print_pass "Non-numeric queue size"
fi

increment_test_count;
if echo -e "wow\n<END>" | ./output/analyzer 0 logger >/dev/null 2>&1; then
    print_fail "Zero queue size"
else
    print_pass "Zero queue size"
fi

increment_test_count;
if echo -e "welp\n<END>" | ./output/analyzer -5 logger >/dev/null 2>&1; then
    print_fail "Negative queue size"
else
    print_pass "Negative queue size"
fi

increment_test_count;
if echo -e "thisisatetststststs\n<END>" | ./output/analyzer 3.1415 logger >/dev/null 2>&1; then
    print_fail "Non-integer queue size"
else
    print_pass "Non-integer queue size"
fi

increment_test_count;
if ./output/analyzer 5 >/dev/null 2>&1; then
    print_fail "Empty plugin list"
else
    print_pass "Empty plugin list"
fi

increment_test_count;
if echo -e "theENDisNear\n<END>" | ./output/analyzer 5 nonexistent_plugin >/dev/null 2>&1; then
    print_fail "Non-existent plugin"
else
    print_pass "Non-existent plugin"
fi

# output tests
print_info "Running output tests"

increment_test_count;
EXPECTED_OUTPUT="[logger] hello
[logger] hell"
OUTPUT=$(echo -e "hello\nhell\n<END>" | ./output/analyzer 5 logger | grep "\[logger\]" 2>/dev/null)
if [[ "$OUTPUT" == "$EXPECTED_OUTPUT" ]]; then
    print_pass "Single plugin pipeline"
else
    print_fail "Single plugin pipeline"
    echo -e "Single plugin pipeline:" >> output/test_output.txt
    echo -e "$OUTPUT" >> output/test_output.txt
fi

increment_test_count;
EXPECTED_OUTPUT_TYPEWRITER="[typewriter] hello
[typewriter] there"
EXPECTED_OUTPUT_LOGGER="[logger] OHELL
[logger] ETHER"
OUTPUT=$(echo -e "hello\nthere\n<END>" | ./output/analyzer 10 typewriter uppercaser rotator logger 2>/dev/null)
OUTPUT_LOGGER=$(echo "$OUTPUT" | grep "\[logger\]")
OUTPUT_TYPEWRITER=$(echo "$OUTPUT" | grep "\[typewriter\]")
if [ "$OUTPUT_TYPEWRITER" == "$EXPECTED_OUTPUT_TYPEWRITER" ] && [ "$OUTPUT_LOGGER" == "$EXPECTED_OUTPUT_LOGGER" ]; then
    print_pass "Multi-plugin pipeline"
else
    print_fail "Multi-plugin pipeline"
    echo -e "Multi-plugin pipeline:" >> output/test_output.txt
    echo -e "$OUTPUT" >> output/test_output.txt
fi

increment_test_count;
EXPECTED_OUTPUT="[typewriter] SETT
[logger] S E T T"
OUTPUT=$(echo -e "test\n<END>" | ./output/analyzer 1 uppercaser rotator flipper typewriter expander logger 2>&1)
if [[ "$OUTPUT" == *"Pipeline shutdown complete"* ]] && [[ "$OUTPUT" == *"[logger]"* ]] && [[ "$OUTPUT" == *"[typewriter]"* ]]; then
    print_pass "All plugins pipeline"
else
    print_fail "All plugins pipeline"
    echo -e "All plugins pipeline:" >> output/test_output.txt
    echo -e "$OUTPUT" >> output/test_output.txt
fi

increment_test_count;
OUTPUT=$(echo -e "<END>" | ./output/analyzer 2 logger 2>/dev/null)
if [[ "$OUTPUT" == "Pipeline shutdown complete" ]]; then
    print_pass "Empty input"
else
    print_fail "Empty input"
    echo -e "Empty input:" >> output/test_output.txt
    echo -e "$OUTPUT" >> output/test_output.txt
fi

increment_test_count;
LONG_STRING=$(printf 'a%.0s' {1..1000})
OUTPUT=$(echo -e "$LONG_STRING\n<END>" | ./output/analyzer 7 uppercaser logger 2>/dev/null)
LOGGER_OUTPUT=$(echo "$OUTPUT" | grep "\[logger\]")
if [[ "$OUTPUT" == *"Pipeline shutdown complete" ]] && \
   [[ "$LOGGER_OUTPUT" == "[logger] $(printf 'A%.0s' {1..1000})" ]]; then
    print_pass "long string"
else
    print_fail "long string"
    echo -e "long string:" >> output/test_output.txt
    echo -e "$OUTPUT" >> output/test_output.txt
fi

if command -v timeout >/dev/null 2>&1; then
    increment_test_count;
    print_info "Testing timeout for EOF input, expected wait time ~5s"
    if timeout 5s bash -c 'echo "hello" | ./output/analyzer 6 logger' >/dev/null 2>&1; then
        print_fail "EOF input"
    else
        print_pass "EOF input"
    fi
else
    print_info "Timeout command not available - skipping"
fi

# additional tests
print_info "Running other tests:"

if command -v valgrind >/dev/null 2>&1; then
    increment_test_count;
    echo -e "test\nline\n<END>" | valgrind --leak-check=full --error-exitcode=1 \
        ./output/analyzer 3 uppercaser logger > output/valgrind_output.txt 2>&1
    if [ $? -eq 0 ]; then
        print_pass "Memory leak detection"
    else
        print_fail "Memory leaks detected"
    fi
else
    print_info "Valgrind not available - skipping"
fi

echo -e "\nResults: $TESTS_PASSED/$TESTS_TOTAL passed"
if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NO_COLOUR}"
    cleanup_test_files
    exit 0
else
    echo -e "${RED}$TESTS_FAILED tests failed${NO_COLOUR}"
    echo -e "Test files preserved for debugging:"
    echo -e "  - output/test_output.txt"
    echo -e "  - output/valgrind_output.txt (if valgrind exists)"
    exit 1
fi
