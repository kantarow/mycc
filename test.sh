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
		exit 1
	fi
}

assert 0 0
assert 42 42

assert 21 "5+20-4"

assert 41 " 12 + 34 - 5 "

assert 42 "4 * 8 + (6 / 3) * 5)"

assert 12 "-(3 + 5) + 20"

assert 3 "1 + +2"

echo OK
