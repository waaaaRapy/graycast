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

assert "main(){0;}" 0
assert "main(){57;}" 57
assert "main(){5+20-4;}" 21
assert "main(){5 + 20 - 4;}" 21
assert "main(){5+6*7;}" 47
assert "main(){5 * (9-6);}" 15
assert "main(){(3+5)/2;}" 4
assert "main(){-1+(+1);}" 0
assert "main(){-3*-5;}" 15
assert "main(){10- -10;}" 20
assert "main(){0 == 0;}" 1
assert "main(){0 != 0;}" 0
assert "main(){0 <= 0;}" 1
assert "main(){0 < 0;}" 0
assert "main(){0 >= 0;}" 1
assert "main(){0 > 0;}" 0
assert "main(){(1<2)+(3>4)+(4<=5<=6)+(7>=8);}" 2
assert "main(){a=12; b=5; a-b+3;}" 10
assert "main(){a=b=c=5; z=a*(b+c); z==a*b+a*c;}" 1
assert "main(){a=12; aa=5; a-aa+3;}" 10
assert "main(){a=A=a_=5; A2=a*(A+a_); A2==a*A+a*a_;}" 1
assert "main(){return1 = 1; return return1; return 2;}" 1
assert "main(){return 8*12-16;}" 80
assert "main(){a=12; if (3*4 > 2*5) a = a * 2; else a = a / 2; return a;}" 24
assert "main(){a=0; while(a<100) a = a + 1; return a;}" 100
assert "main(){r=12; i=-12; whi=3; return r + i + whi;}" 3
assert "main(){sum=0; i=0; while(i<=10) {sum = sum + i; i = i + 1;} return sum;}" 55
assert "main(){sum=0; for(i=0; i<=10; ++i) sum += i; return sum;}" 55
assert "main(){return fibo(10);} fibo(n){if(n<=1) return 1; else return fibo(n-1)+fibo(n-2);}" 89
assert "main(){a=19940427;while(a!=4){if(a%2==0){a/=2;} else {a*=3;++a;}} return a;}" 4

echo OK
