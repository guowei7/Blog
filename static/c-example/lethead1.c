#include <stdio.h>

#define NAME "GIGATHINK, INC."

#define ADDRESS "101 Megabuck Plaza"

#define PLACE "Megapolis, CA 94904"

#define WIDTH 40

void starbar(void); 

int main(void)
{
	starbar();
	printf("%s\n", NAME);
	printf("%s\n", ADDRESS);
	printf("%s\n", PLACE);
	starbar();
	return 0;
}

/**
 * fn       starbar
 * authoer  wguo
 * date     2021-01-11
 * brief    printf start
 * relates  no
 * remarks  no
 */
void starbar(void)
{
	int count;
	for (int count = 1; count < WIDTH; count++)
	{
		putchar('*');
	}
	putchar('\n');
}