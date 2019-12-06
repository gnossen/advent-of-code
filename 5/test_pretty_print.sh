#!/bin/bash

function test_file_pair () {
  INPUT="$1"
  EXPECTED="$2" 
  TEMPFILE=$(mktemp)
  pretty_print "$INPUT" >"$TEMPFILE"
  RET="$?"
  if [ "$RET" != "0" ]; then
    echo "FAILURE" >/dev/stderr
    echo "pretty_print failed with code ${RET}" >/dev/stderr
    exit 1
  fi
  diff "$EXPECTED" "$TEMPFILE"
  if [ "$?" != "0" ]; then
    echo "FAILURE" > /dev/stderr
    exit 1
  fi
  echo "SUCCESS"
}

test_file_pair 4.1202 4.pretty.1202
test_file_pair 5.1202 5.pretty.1202
test_file_pair 6.1202 6.pretty.1202
test_file_pair 7.1202 7.pretty.1202
