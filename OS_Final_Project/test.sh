#!/bin/bash
set -e

source ./build.sh

echo -e \"\\nRunning basic test...\"

EXPECTED=\"[logger] HELLO\"
ACTUAL=$(echo \"hello\n<END>\" | ./output/analyzer 10 uppercaser logger | grep \"\\[logger\\]\")

if [ \"$ACTUAL\" == \"$EXPECTED\" ]; then
    echo \"[TEST] PASS\"
else
    echo \"[TEST] FAIL - got '$ACTUAL'\"
    exit 1
fi
