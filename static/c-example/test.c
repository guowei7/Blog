#include <stdio.h>
#include <string.h>
int main(void)
{
    int a=0,i;
    for(i=1;i<5;i++)
    {
    	switch(i)
    	{
    		case 0:
    		case 3: a+=2;
    		case 1:
    		case 2: a+=3;
    		default: a+=5;
    	}
    }
    printf("%d\n",a);
    printf("%d\n",sizeof(void *));
}
