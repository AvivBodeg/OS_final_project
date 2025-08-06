#!/bin/bash
set -e

# Colors
RED='\\033[0;31m'
GREEN='\\033[0;32m'
YELLOW='\\033[1;33m'
NC='\\033[0m'

print_status() { echo -e \"${GREEN}[BUILD]${NC} $1\"; }
print_error() { echo -e \"${RED}[ERROR]${NC} $1\"; }

mkdir -p output

print_status \"Building main application...\"
gcc -o output/analyzer main.c -ldl -lpthread
print_status \"Build complete.\"
