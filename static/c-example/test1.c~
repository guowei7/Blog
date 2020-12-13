#include <stdio.h>

int main(void)
{
	struct obj
	{
		char a;
		int b;
		unsigned char c;
		unsigned long long int d[2];
	};
	struct obj abc;
	printf("a->%d,b->%d,c->%d,d->%d\n",
		(unsigned int)(void*)&abc.a-(unsigned int)(void *)&abc,
		(unsigned int)(void*)&abc.b-(unsigned int)(void *)&abc,
		(unsigned int)(void*)&abc.c-(unsigned int)(void *)&abc,
		(unsigned int)(void*)&abc.d-(unsigned int)(void *)&abc);
	printf("%d",sizeof(abc));		
	return 0;
}
