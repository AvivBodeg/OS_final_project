#!/bin/bash

# colours
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NO_COLOUR='\033[0m'

TESTS_PASSED=0
TESTS_FAILED=0
TESTS_TOTAL=0

# unique timestamp for broken plugin test
TIMESTAMP=$(date +%s)
BROKEN_PLUGIN_NAME="broken_test_${TIMESTAMP}"

# printing functions
print_success() {
    echo -e "$1: ${GREEN}PASS${NO_COLOUR}"
    ((TESTS_PASSED++))
}

print_failure() {
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
    rm -f output/${BROKEN_PLUGIN_NAME}.so
    rm -f plugins/${BROKEN_PLUGIN_NAME}.c
    rm -f test_output.txt
    rm -f valgrind_output.txt
}

# build
print_info "Building project..."
if ! ./build.sh > /dev/null 2>&1; then
    echo -e "${RED}[ERROR]${NO_COLOUR} Build failed"
    exit 1
fi
print_info "Build succeeded"

# input validation tests
print_info "Running validation tests"

increment_test_count;
if echo -e "test\n<END>" | ./output/analyzer aviv:')' logger >/dev/null 2>&1; then
    print_failure "Non-numeric queue size"
else
    print_success "Non-numeric queue size"
fi

increment_test_count;
if echo -e "wow\n<END>" | ./output/analyzer 0 logger >/dev/null 2>&1; then
    print_failure "Zero queue size"
else
    print_success "Zero queue size"
fi

increment_test_count;
if echo -e "welp\n<END>" | ./output/analyzer -5 logger >/dev/null 2>&1; then
    print_failure "Negative queue size"
else
    print_success "Negative queue size"
fi

increment_test_count;
if echo -e "thisisatetststststs\n<END>" | ./output/analyzer 3.1415 logger >/dev/null 2>&1; then
    print_failure "Non-integer queue size"
else
    print_success "Non-integer queue size"
fi

increment_test_count;
if ./output/analyzer 5 >/dev/null 2>&1; then
    print_failure "Empty plugin list"
else
    print_success "Empty plugin list"
fi

increment_test_count;
if echo -e "theENDisNear\n<END>" | ./output/analyzer 5 nonexistent_plugin >/dev/null 2>&1; then
    print_failure "Non-existent plugin"
else
    print_success "Non-existent plugin"
fi

# broken plugin test (and creation of temp broken plugin)
cat > plugins/${BROKEN_PLUGIN_NAME}.c << 'EOF'
const char* plugin_get_name(void) { return "broken_test"; }
EOF
if gcc -fPIC -shared -o output/${BROKEN_PLUGIN_NAME}.so plugins/${BROKEN_PLUGIN_NAME}.c 2>/dev/null; then
    if echo -e "imoutofideas\n<END>" | ./output/analyzer 5 ${BROKEN_PLUGIN_NAME} >/dev/null 2>&1; then
        print_failure "Broken plugin"
        increment_test_count;
    else
        print_success "Broken plugin"
        increment_test_count;
    fi
else
    print_info "Failed to create broken plugin - skipping"
fi

# output tests
print_info "Running output tests"

increment_test_count;
EXPECTED_OUTPUT="[logger] hello
[logger] hell"
OUTPUT=$(echo -e "hello\nhell\n<END>" | ./output/analyzer 5 logger | grep "\[logger\]" 2>/dev/null)
if [[ "$OUTPUT" == "$EXPECTED_OUTPUT" ]]; then
    print_success "Single plugin pipeline"
else
    print_failure "Single plugin pipeline"
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
    print_success "Multi-plugin pipeline"
else
    print_failure "Multi-plugin pipeline"
fi

increment_test_count;
EXPECTED_OUTPUT="[typewriter] SETT
[logger] S E T T"
OUTPUT=$(echo -e "test\n<END>" | ./output/analyzer 1 uppercaser rotator flipper typewriter expander logger 2>&1)
if [[ "$OUTPUT" == *"Pipeline shutdown complete"* ]] && [[ "$OUTPUT" == *"[logger]"* ]] && [[ "$OUTPUT" == *"[typewriter]"* ]]; then
    print_success "All plugins pipeline"
else
    print_failure "All plugins pipeline"
fi

increment_test_count;
OUTPUT=$(echo -e "<END>" | ./output/analyzer 2 logger 2>/dev/null)
if [[ "$OUTPUT" == "Pipeline shutdown complete" ]]; then
    print_success "Empty input"
else
    print_failure "Empty input"
fi

increment_test_count;
LONG_STRING=$(printf 'a%.0s' {1..1000})
OUTPUT=$(echo -e "$LONG_STRING\n<END>" | ./output/analyzer 7 uppercaser logger 2>/dev/null)
LOGGER_OUTPUT=$(echo "$OUTPUT" | grep "\[logger\]")
if [[ "$OUTPUT" == *"Pipeline shutdown complete" ]] && \
   [[ "$LOGGER_OUTPUT" == "[logger] $(printf 'A%.0s' {1..1000})" ]]; then
    print_success "long string"
else
    print_failure "long string"
fi

if command -v timeout >/dev/null 2>&1; then
    increment_test_count;
    print_info "Testing timeout for EOF input, expected wait time ~5s"
    if timeout 5s bash -c 'echo "hello" | ./output/analyzer 6 logger' >/dev/null 2>&1; then
        print_failure "EOF input"
    else
        print_success "EOF input"
    fi
else
    print_info "Timeout command not available - skipping"
fi

# additional tests
print_info "Running other tests:"

if command -v valgrind >/dev/null 2>&1; then
    increment_test_count;
    echo -e "test\nline\n<END>" | valgrind --leak-check=full --error-exitcode=1 \
        ./output/analyzer 3 uppercaser logger > valgrind_output.txt 2>&1
    if [ $? -eq 0 ]; then
        print_success "Memory leak detection"
    else
        print_failure "Memory leaks detected"
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
    echo -e "  - plugins/${BROKEN_PLUGIN_NAME}.c"
    echo -e "  - output/${BROKEN_PLUGIN_NAME}.so"
    echo -e "  - valgrind_output.txt (if valgrind exists)"
    exit 1
fi
