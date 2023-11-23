#!/bin/bash

set -e

cd $(dirname -- "${BASH_SOURCE[0]}")

echo "Test pretty_printer"
./test_pretty_print.sh

echo "Test buffer data structure"
./buffer_test

echo "Test Interpreter"
./test_interpreter.sh

echo "Test Bytecode"
./bytecode_test
