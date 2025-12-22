#!/bin/bash

LAMAC=lamac
ASSIGNMENT02="$(pwd)/build/assignment02"
DIFF=diff
TIME="/usr/bin/time"
RUNTIME_DIR="$(pwd)/runtime"
REGRESSION_TESTS_DIR="$(pwd)/tests/regression"
PERFORMANCE_TESTS_DIR="$(pwd)/tests/performance"
FAILED_TESTS=()

print_status_and_record () {
  local test_name="$1"
  local expected_output="$2"
  local actual_output="$3"
  local status=$4
  if [ $status -ne 0 ]; then
    FAILED_TESTS+=("$test_name")
    echo "$test_name: FAILED"
    echo "Expected $(cat "$expected_output"), got $(cat "$actual_output") instead!"
  else
    echo "$test_name: SUCCESS"
  fi
  echo
}

print_time () {
  local test_name="$1"
  local recursive_source_level_time=$2
  local recursive_bytecode_time=$3
  local iterative_bytecode_time=$4
  echo "$test_name:"
  echo -e "Recursive source-level interpreter\t$recursive_source_level_time"
  echo -e "Recursive bytecode interpreter\t$recursive_bytecode_time"
  echo -e "Iterative bytecode interpreter\t$iterative_bytecode_time"
  echo
}

regression_test () {
  local test_name="$1"
  local test_bytecode="${test_name%.*}.bc"
  local test_input="${test_name%.*}.input"
  local expected_output="${test_name%.*}.expected.output"
  local actual_output="${test_name%.*}.actual.output"
  "$LAMAC" -I "$RUNTIME_DIR" -b "$test_name" > /dev/null 2>&1
  if [ $? -ne 0 ]; then
    echo "$test_name: COMPILATION FAILED"
    echo
    return $?
  fi
  "$LAMAC" -I "$RUNTIME_DIR" -i "$test_name" < "$test_input" > "$expected_output" 2>&1 && \
  "$ASSIGNMENT02" "$test_bytecode" < "$test_input" > "$actual_output" 2>&1 && \
  "$DIFF" -q "$expected_output" "$actual_output" > /dev/null 2>&1
  local result=$?
  print_status_and_record "$test_name" "$expected_output" "$actual_output" $result
  rm -f "$test_bytecode" "$expected_output" "$actual_output"
  return $result
}

performance_test () {
  local test_name="$1"
  local test_bytecode="${test_name%.*}.bc"
  "$LAMAC" -I "$RUNTIME_DIR" -b "$test_name" > /dev/null 2>&1
  if [ $? -ne 0 ]; then
    echo "$test_name: COMPILATION FAILED"
    echo
    return
  fi
  local recursive_source_level_time=$("$TIME" -f %U "$LAMAC" -I "$RUNTIME_DIR" -i "$test_name" < /dev/null 2>&1 > /dev/null)
  local recursive_bytecode_time=$("$TIME" -f %U "$LAMAC" -I "$RUNTIME_DIR" -s "$test_name" < /dev/null 2>&1 > /dev/null)
  local iterative_bytecode_time=$("$TIME" -f %U "$ASSIGNMENT02" "$test_bytecode" < /dev/null 2>&1 > /dev/null)
  print_time "$test_name" $recursive_source_level_time $recursive_bytecode_time $iterative_bytecode_time
  rm -f "$test_bytecode"
}

echo "Running regression tests"
cd "$REGRESSION_TESTS_DIR"
for f in *.lama; do
  regression_test "$f"
done
if [ ${#FAILED_TESTS[@]} -eq 0 ]; then
  echo "All tests succeeded!"
else
  echo "${#FAILED_TESTS[@]} tests failed:"
  (IFS=$'\n'; echo "${FAILED_TESTS[*]}")
  exit 1
fi

echo "Running performance tests"
cd "$PERFORMANCE_TESTS_DIR"
for f in *.lama; do
  performance_test "$f"
done
