#!/bin/bash

LAMAC=lamac
ASSIGNMENT05="$(pwd)/build/install/Assignment05/bin/Assignment05"
DIFF=diff
TIME="/usr/bin/time"
RUNTIME_DIR="$(pwd)/../runtime"
REGRESSION_TESTS_DIR="$(pwd)/../tests/regression"
PERFORMANCE_TESTS_DIR="$(pwd)/../tests/performance"
DISABLED_TESTS=("test092.lama" "test094.lama" "test095.lama" "test096.lama" "test098.lama" "test105.lama" "test111.lama")
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
  local graal_truffle_time=$4
  echo "$test_name:"
  echo -e "Recursive source-level interpreter\t$recursive_source_level_time"
  echo -e "Recursive bytecode interpreter\t$recursive_bytecode_time"
  echo -e "Graal Truffle interpreter\t$graal_truffle_time"
  echo
}

regression_test () {
  local test_name="$1"
  local test_input="${test_name%.*}.input"
  local expected_output="${test_name%.*}.expected.output"
  local actual_output="${test_name%.*}.actual.output"
  "$LAMAC" -I "$RUNTIME_DIR" -i "$test_name" < "$test_input" > "$expected_output" 2>&1 && \
  "$ASSIGNMENT05" "$test_name" < "$test_input" > "$actual_output" 2>&1 && \
  "$DIFF" -q "$expected_output" "$actual_output" > /dev/null 2>&1
  local result=$?
  print_status_and_record "$test_name" "$expected_output" "$actual_output" $result
  rm -f "$expected_output" "$actual_output"
  return $result
}

performance_test () {
  local test_name="$1"
  local recursive_source_level_time=$("$TIME" -f %U "$LAMAC" -I "$RUNTIME_DIR" -i "$test_name" < /dev/null 2>&1 > /dev/null)
  local recursive_bytecode_time=$("$TIME" -f %U "$LAMAC" -I "$RUNTIME_DIR" -s "$test_name" < /dev/null 2>&1 > /dev/null)
  local graal_truffle_time=$("$TIME" -f %U "$ASSIGNMENT05" "$test_name" < /dev/null 2>&1 > /dev/null)
  print_time "$test_name" $recursive_source_level_time $recursive_bytecode_time $graal_truffle_time
}

echo "Running regression tests"
cd "$REGRESSION_TESTS_DIR"
for f in *.lama; do
  if [[ ! "${DISABLED_TESTS[*]}" =~ "$f" ]]; then
    regression_test "$f"
  fi
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
