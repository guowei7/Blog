#include <stdio.h>

long fact(int n);

long rfact(int n);

int main(void)
{
    int num;
    printf("This program calculates factorials.\n");
    printf("Enter a value int the range 0-12 (q to quit):\n");
    while(scanf("%d", &num) == 1)
    {
        if(num < 0)
            printf("No negative numbers,please.\n");
        else if(num > 12)
            printf("Keep input under 13.\n");
        else
        {
            printf("loop: %d factorial = %ld\n", num, fact(num));
            printf("recursion %d factorial = %ld\n", num, rfact(num));
        }
        printf("Enter a value in the range (q to quit):\n");
    }
    printf("Bye.\n");
    return 0;
}

/**
 * fn       fact
 * authoer  wguo
 * date     2021-01-24
 * brief    cal factorial of n
 * relates  no
 * remarks  no
 * @param  n a num from 0 to 12
 * @return   the factorial of n
 */
long fact(int n)
{
	long ans;
	for(ans = 1;n>1;n--)
		ans *= n;
	return ans;
}

/**
 * fn       rfact
 * authoer  wguo
 * date     2021-01-24
 * brief    cal factorial of n by recursion
 * relates  no
 * remarks  no
 * @param  n the num is used cal
 * @return   the faacctorial of num
 */
long rfact(int n)
{
	long ans;
	if(n > 0)
	{
		ans = n*rfact(n-1);
	}
	else
	{
		ans = 1;
	}
	return ans;
}