#!/bin/bash

function test_once () {
    EXECUTABLE="$1"
    INPUT_FILE="$2"
    INPUT_ARGUMENT="$3"
    EXPECTED="$4"
    echo "case $INPUT_FILE $INPUT_ARGUMENT"
    ANSWER=$("$EXECUTABLE" "$INPUT_FILE" "$INPUT_ARGUMENT")
    RC="$?"
    if [ "$RC" != "0" ]; then
        echo "FAILURE" >/dev/stderr
        echo "$1 exited with code $RC" >/dev/stderr
        exit 1
    fi
    if [ "$ANSWER" != "${EXPECTED}" ]; then
        echo "FAILURE" >/dev/stderr
        echo "\"${ANSWER}\" was not \"${EXPECTED}\"." >/dev/stderr
        exit 1
    fi

    echo "SUCCESS"
}

if [ "$#" -ne "1" ]; then
    echo "Usage: $0 executable" >/dev/stderr
    exit 1
fi

test_once "$1" input.1202 19690720 5696
