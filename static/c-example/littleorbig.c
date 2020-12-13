#include<stdio.h>
#include<stdlib.h>

int main()
{
	union{
		short s;
		char c[sizeof(short)];
	}u;
	u.s = 0x0102;
	if(u.c[0] == 1&&u.c[1] == 2)
	{
		printf("big-end\n");
	}else if(u.c[0] == 2&&u.c[1] ==1)
	{
		printf("little-end\n");
	}else
	{
		printf("unknow\n");
	}
	printf("%c\n",80);
	return 0;
}
