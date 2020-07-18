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

assert "0;" 0
assert "57;" 57
assert "5+20-4;" 21
assert "5 + 20 - 4;" 21
assert "5+6*7;" 47
assert "5 * (9-6);" 15
assert "(3+5)/2;" 4
assert "-1+(+1);" 0
assert "-3*-5;" 15
assert "10--10;" 20
assert "0 == 0;" 1
assert "0 != 0;" 0
assert "0 <= 0;" 1
assert "0 < 0;" 0
assert "0 >= 0;" 1
assert "0 > 0;" 0
assert "(1<2)+(3>4)+(4<=5<=6)+(7>=8);" 2
assert "a=12; b=5; a-b+3;" 10
assert "a=b=c=5; z=a*(b+c); z==a*b+a*c;" 1
assert "a=12; aa=5; a-aa+3;" 10
assert "a=A=a_=5; A2=a*(A+a_); A2==a*A+a*a_;" 1
assert "return1 = 1; return return1; return 2;" 1
assert "return 8*12-16;" 80
assert "a=12; if (3*4 > 2*5) a = a * 2; else a = a / 2; return a;" 24
assert "a=0; while(a<100) a = a + 1; return a;" 100
assert "r=12; i=-12; whi=3; return r + i + whi;" 3
assert "sum=0; i=0; while(i<=10) {sum = sum + i; i = i + 1;} return sum;" 55
echo OK
