#!/bin/bash

function test_interpreter () {
  PROGRAM="$1"
  INPUT="$2"
  EXPECTED="$3"
  TEMPFILE=$(mktemp)
  printf "$INPUT" | ./interpreter "$PROGRAM" >"$TEMPFILE"
  RET="$?"
  if [ "$RET" != "0" ]; then
    echo "FAILURE" >/dev/stderr
    echo "interpreter failed with code ${RET}" >/dev/stderr
    exit 1
  fi
  diff "$EXPECTED" "$TEMPFILE"
  if [ "$?" != "0" ]; then
    echo "FAILURE" > /dev/stderr
    exit 1
  fi
  echo "SUCCESS"
}

test_interpreter 7.1202 '1\n2\n3\n' 7.output
test_interpreter 6.1202 '' 6.output
test_interpreter 8.1202 '' 8.output
test_interpreter 10.1202 '' 10.output
test_interpreter 11.1202 '' 11.output
test_interpreter 12.1202 '' 12.output
test_interpreter 13.1202 '' 13.output
test_interpreter 14.1202 '' 14.output
