#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -g -o tmp tmp.s
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

assert 46 '
foo=11;
bar=12;
cal=(foo+bar)*2;
'

assert 70 '
var1=10;
var2=20;
var3=var1+3*var2;
return var3;
'

echo OK