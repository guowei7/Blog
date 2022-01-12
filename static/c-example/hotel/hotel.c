/**
 * hotel.c 酒店管理函数
 */
#include <stdio.h>
#include "hotel.h"

int menu(void)
{
    int code, status;

    printf("\n%s%s\n", STARS, STARS);
    printf("Enter the number of the desired hotel:\n");
    printf("1) Fairfiled Arms(%.2f)	2) Hotel Olympic(%.2f)\n", HOTEL1, HOTEL2);
    printf("3) Chertworthy Plaza(%.2f)	4) The Stockton(%.2f)\n", HOTEL3, HOTEL4);
    printf("5) quit\n");
    printf("%s%s\n", STARS, STARS);
    while((status = scanf("%d", &code)) != 1 || (code < 1 || code > 5))
    {
        if(status != 1)
            scanf("%*s");//处理非整数输入
        printf("Enter an integer from 1 to 5,Please.\n");
    }
    return code;
}

int getnights(void)
{
    int nights;
    printf("How many nights are needed?\n");
    while(scanf("%d", &nights) != 1)
    {
        scanf("%*s");//处理非整数输入
        printf("Please enter an integer ,such as 2.\n");
    }
    return nights;
}

void showprice(double rate, int nights)
{
    int n;
    double total = 0.0;
    double factor = 1.0;
    for(n = 1; n <= nights; n++, factor *= DISCOUNT)
        total += rate * factor;
    printf("The total cost will be $%0.2f.\n", total);
}