#!/bin/bash

cat <<EOF | gcc -ggdb -xc -S -o tmp2.s - 
#include <stdio.h>
int ret3() {return 3;}
int ret5() {return 5;}
void print_ok() {printf("OK\n");}
int  print_add(int x, int y) {printf("%d",x+y); return x + y ;}
int add_6(int x1,int x2, int x3, int x4, int x5, int x6) {return x1+x2+x3+x4+x5+x6;}
EOF



assert() {
	expected="$1"
	input="$2"
	./9cc "$input" > tmp.s
	gcc -ggdb -static -o tmp tmp.s tmp2.s
	./tmp
	actual="$?"
	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else 
		echo "$input => $expected expected, but got $actual"
		exit 0
	fi
}
# assert 0 "0;"
# assert 42 "42;"
# assert 41 " 12 + 34 - 5; "
# assert 7 "1 + 2 * 3;"
# assert 47 '5+6*7;'
# assert 15 '5*(9-6);'
# assert 4 '(3+5)/2;'
# assert 10 '-10+20;'
# assert 200 '-10*-20; '
# assert 100 '-10*-20 - -10*-10; '
# assert 200 '-10*-20 - -10*-10  + 10*+10;'
# assert 1   '3 < 4;'
# assert 1  ' 3 <=4;'
# assert 0  '3 > 4;'
# assert 0 '3 >=4;'
# assert 1 '1+20 == 20 +1; '
# assert 1 '20 != 21;'
# assert 3 '1;2;3;'
# assert 10 'a = 10; a;'
# assert 10 'a =3; b=7; a+b;'
# assert 10 'aa =3; bb=7; aa+bb;'
# assert 10 "return 10;"
# assert 10 "return 4+6;"
# assert 10 'aa =3; bb=7; return aa+bb;'
# assert 3 'aa =3; return aa; bb=7; return aa+bb;'
# assert 3 '{return 3;}'
# assert 3 '{aa =3; return aa; bb=7; return aa+bb;}'
# assert 3 '{aa =3; return aa;}{ bb=7; return aa+bb;}'
# assert 3  'if (1) return 3; else return 4;' 
# assert 4  'if (0) return 3; else return 4;' 
# assert 3  'aa = 3; if(aa ==3) return aa; else return aa + 1;'
# assert 4  'aa = 3; if(aa !=3) return aa; else return aa + 1;'
# assert 3  'a  = 0; while(a < 3) a = a + 1;  return a;'
# assert 3  ';; return 3;'
# assert 3  'b = 0 ; for(a=0; a<3; a=a+1) b = b+a; return b;'
# assert 6  'b = 0 ; for(a=0; a<4; a=a+1) b = b+a; return b;'
# assert 6  'b = 0 ; a =0 ; for(; a<4; a=a+1) b = b+a; return b;'
# assert 6  'b = 0 ; a =0 ; for(; a<4; ) {b = b+a; a= a+1;} return b;'
# assert 3  'b = 0 ; a =0 ; for(; ; ) {return 3;} return b;'
assert 3  'return ret3();'
assert 5  'return ret5();'
assert 8  'a = ret3(); b = ret5(); return a+b;'
assert 3  'print_ok(); print_ok(); return 3;'
assert 3  'print_ok(); print_ok(); return 3;'
assert 21  'a = add_6(1,2,3,4,5,6); return a;'
assert 8  'a = print_add(3,5); return a;'
echo OK