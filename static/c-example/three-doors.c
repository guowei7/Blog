#include<stdio.h>
#include<stdlib.h>

#define LOOP_NUM 100000
int main(void)
{
	int carindex = 0;
	int doors[3] = {0};
	int count = 1;
	int chooseindex = 0;
	int changebingonum = 0;
	while(count++ <= LOOP_NUM)
	{
		doors[0] = 0;
		doors[1] = 0;
		doors[2] = 0;	
		carindex = rand() % 3;
		doors[carindex] = 1;
		printf("carindex = %d\n",carindex);
		if(doors[1] == 0)
		{
			printf("1 is sheep,change choose!\n");
			chooseindex = 2;
		}
		else
		{
			printf("2 is sheep,change choose!\n");
			chooseindex = 1;
		}
		if(doors[chooseindex] == 1)
			changebingonum++;
	}
	printf("changebingonum = %d,accuracy = %.4f%%",changebingonum,(float)changebingonum / LOOP_NUM*100.0);
	return 0;
}