#!/bin/bash
set -euo pipefail

  RED='\033[0;31m'
  GREEN='\033[0;32m'
  YELLOW='\033[1;33m'
  NC='\033[0m'

print_status()
{ 
  echo -e "${GREEN}[BUILD]${NC} $*"; 
}
print_warning()
{
   echo -e "${YELLOW}[WARN] ${NC} $*";
   }
print_error()
{
  echo -e "${RED}[ERROR]${NC} $*";
}

mkdir -p output

print_status "Building plugins..."

for plugin_name in logger uppercaser rotator flipper expander typewriter; do
  print_status "Building plugin: $plugin_name"
  gcc -fPIC -shared -o output/${plugin_name}.so \
    plugins/${plugin_name}.c \
    plugins/plugin_common.c \
    plugins/sync/monitor.c \
    plugins/sync/consumer_producer.c \
    -ldl -lpthread || {
    print_error "Failed to build $plugin_name"
    exit 1
  }
done

print_status "Building main application..."
gcc -Iplugins -Wall -Wextra -o output/analyzer main.c -ldl -lpthread || {
  print_error "Failed to build main application"
  exit 1
}

print_status "Build complete."
print_status "Plugins built: $(ls output/*.so 2>/dev/null | wc -l)"
print_status "Main executable: output/analyzer"
