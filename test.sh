#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./ycc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}
try 0 0
try 42 42
try 21 '5+20-4'
try 17 '3 + 4 - 20 + 30'
try 47 "5+6*7"
try 15 "5*(9-6)"
try 4 "(3+5)/2"
try 105  "a=5;a+100"
try 22 "3*5+7"
try 23 "1+(3*5+7)"
try 25 "a=3*5+7;a+3"
try 54 "a=3*5+7;b=3+4+5;c=20;a+b+c"
try 10 "a=3+4+3;b=5*4;a+b-20"
try 1 "1==1"
try 0 "1==2"
try 1 '1!=2'
try 10 'a=(((3+4)==7)*10);a'
try 4 'a=(((3+4)!=7)*10);a+4'
echo OK
