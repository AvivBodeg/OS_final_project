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

print_status "Building main application…"
gcc -Iplugins -Wall -Wextra -o output/analyzer main.c -ldl -lpthread

print_status "Build complete."
