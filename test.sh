#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
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

assert 12 '
a=10;
b=2;
c=a+b;
'

assert 120 '
a=10;
b=a*a;
c=2*a+b;'

echo OK