#include <stdio.h>
int ret3() {return 3;}
int ret5() {return 5;}
int ret_add(int x, int y, int z) {printf("%d", x+y+z);return x + y;}
int main(int argc, char* argv[])
{
	int a = ret3();
	int b = ret5();
	return ret_add(a,b,3);
}
