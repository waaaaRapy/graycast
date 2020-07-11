#!/bin/bash

assert() {
  input="$1"
  expected="$2"

  ./graycast "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert "0" 0
assert "57" 57
assert "5+20-4" 21
assert "5 + 20 - 4" 21
assert "5+6*7" 47
assert "5 * (9-6)" 15
assert "(3+5)/2" 4
assert "-1+(+1)" 0
assert "-3*-5" 15
assert "10--10" 20
assert "0 == 0" 1
assert "0 != 0" 0
assert "0 <= 0" 1
assert "0 < 0" 0
assert "0 >= 0" 1
assert "0 > 0" 0
assert "(1<2)+(3>4)+(4<=5<=6)+(7>=8)" 2
echo OK
