#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc $input > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" == "$expected" ]; then
        echo "$input => $expected"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 12 12
assert 42 42
assert 21 "5+20-4"

echo OK