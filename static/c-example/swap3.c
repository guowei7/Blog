#include <stdio.h>

void interchange(int *u,int *v);

int main(void)
{
	int x = 5, y = 10;
	printf("Originally x = %d and y = %d.\n", x, y);
	interchange(&x,&y);
	printf("Now x = %d and y = %d.\n",x, y);
	return 0;
}

/**
 * fn       interchange
 * authoer  wguo
 * date     2021-02-06
 * brief    switch two num
 * relates  no
 * remarks  noo
 * @param u first num address
 * @param v sec num address
 */
void interchange(int *u,int *v)
{
	int temp;
	temp = *u;
	*u = *v;
	*v = temp;
}