#include <stdio.h>

void to_binary(unsigned long n);

int main(void)
{
	unsigned long number;
	printf("Enter an integer (q to quit)\n");
	while(scanf("%lu",&number) == 1)
	{
		printf("Binary equivalent: ");
		to_binary(number);
		putchar('\n');
		printf("Enter an integer (q to quit):\n");
	}
	printf("Done.\n");
	return 0;
}

/**
 * fn       to_binary
 * authoer  wguo
 * date     2021-01-25
 * brief    cal binary of a decimal number
 * relates  no
 * remarks  no
 * @param n [a decimal number]
 */
void to_binary(unsigned long n)
{
	if(n >= 2)
	{
		to_binary(n / 2);
	}
	putchar(n % 2 ? '1' : '0');
	return;
}