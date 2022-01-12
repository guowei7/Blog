#include <stdio.h>

void up_and_down(int);

int main(void)
{
    up_and_down(1);
    return 0;
}

/**
 * fn       up_and_down
 * authoer  wguo
 * date     2021-01-23
 * brief    show recursion mem address
 * relates  no
 * remarks  no
 * @param n the num of recursion level
 */
void up_and_down(int n)
{
    printf("Level %d: n location %p\n", n, &n);
    if(n < 4)
    {
        up_and_down(n + 1);
    }
    printf("LEVEL %d: n location %p\n", n, &n);
}