#!/bin/bash
assert() {
	expected="$1"
	input="$2"

	./mycc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
	fi
}

<< COMMENTOUT
assert 0 "0;"
assert 42 "42;"

assert 21 "5+20-4;"

assert 41 " 12 + 34 - 5 ;"

assert 42 "4 * 8 + (6 / 3) * 5;"

assert 12 "-(3 + 5) + 20;"

assert 3 "1 + +2;"

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

COMMENTOUT

assert 1 '34<32+6;'

assert 3 '3<4;1+2;'

assert 10 'a=2; b=3; c=4; a*b+c;'

assert 5 'foo = 2; bar = 3; foo + bar;'

assert 1 'nyan = 2; foo = 5; bar = 3; nyan == foo - bar;'

assert 3 'return 3;'
assert 6 'nyan = 3; return nyan * 2;'

assert 5 'a = 3; b = 6; c = 4; if(a < b) {c = 5;} return c;'

assert 1 'a = 1; b = 2; c = 3; if (a + b == c) { c = 4; if (b < a * c) { c = c * 2 - (a + b + 4); } } return c;'

assert 2 'a = 1; b = 2; c = 3; if (a + b != c) { c = 1; } else { c = 2; } return c;'

assert 2 'a = 1; b = 2; c = 3; if (a + b != c) { c = 1; } else {  if (a  * b < c) { c = 2; }} return c;'

assert 4 'a = 1; a = a + 3; return a;'

assert 5 'sum = 0; while (sum < 5) { sum = sum + 1; } return sum;'

assert 120 'a = 1; counter = 1; end = 5; while (counter <= end) { a = a * counter; counter = counter + 1; } return a;'

assert 8 'for (i=1; i<5; i=i) { i=i*2; } return i;'
assert 55 'sum = 0; for (i=0; i<=10; i=i+1) { sum = sum + i; } return sum;'

echo OK
