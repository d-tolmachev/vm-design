#!/bin/bash

LAMAC=lamac
ASSIGNMENT03="$(pwd)/build/assignment03"
RUNTIME_DIR="$(pwd)/../runtime"
TESTS_DIR="$(pwd)/../tests/performance"

test () {
  local test_name="$1"
  local test_bytecode="${test_name%.*}.bc"
  "$LAMAC" -I "$RUNTIME_DIR" -b "$test_name" > /dev/null 2>&1
  if [ $? -ne 0 ]; then
    echo "$test_name: COMPILATION FAILED"
    echo
    return
  fi
  echo "$test_name:"
  "$ASSIGNMENT03" "$test_bytecode" < /dev/null
  rm -f "$test_bytecode"
}

cd "$TESTS_DIR"
for f in *.lama; do
  test "$f"
done
