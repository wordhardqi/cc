#!/bin/bash
assert() {
	expected="$1"
	input="$2"
	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"
	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else 
		echo "$input => $expected expected, but got $actual"
		exit 0
	fi
}
assert 0 "0;"
assert 42 "42;"
assert 41 " 12 + 34 - 5; "
assert 7 "1 + 2 * 3;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 200 '-10*-20; '
assert 100 '-10*-20 - -10*-10; '
assert 200 '-10*-20 - -10*-10  + 10*+10;'
assert 1   '3 < 4;'
assert 1  ' 3 <=4;'
assert 0  '3 > 4;'
assert 0 '3 >=4;'
assert 1 '1+20 == 20 +1; '
assert 1 '20 != 21;'
assert 3 '1;2;3;'
assert 10 'a = 10; a;'
assert 10 'a =3; b=7; a+b;'
assert 10 'aa =3; bb=7; aa+bb;'
assert 10 "return 10;"
assert 10 "return 4+6;"
assert 10 'aa =3; bb=7; return aa+bb;'
assert 3 'aa =3; return aa; bb=7; return aa+bb;'
assert 3 '{return 3;}'
echo OK