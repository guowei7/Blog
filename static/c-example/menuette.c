#include <stdio.h>

char get_choice(void);
char get_first(void);
int get_int(void);
void count(void);

int main(void)
{
	int choice;
	while((choice = get_choice()) != 'q')
	{
		switch(choice)
		{
			case 'a':
				printf("Buy low, sell high.\n");
				break;
			case 'b':
				putchar('\a');
				break;
			case 'c':
				count();
				break;
			default:
				printf("Program error!\n");
				break;
		}
	}
	printf("Bye.\n");
	return 0;
}

char get_choice(void)
{
	int ch;
	printf("Enter the letter of your choice:\n");
	printf("a. advice       b. bell\n");
	printf("c. count        q. quit\n");
	ch = get_first();
	while((ch < 'a' || ch > 'c') && ch != 'q')
	{
		printf("Please respond with a, b, c, or q.\n");
		ch = get_first();
	}
	return ch;
}

char get_first()
{
	int ch;
	ch = getchar();
	while(getchar() != '\n')
		continue;
	return ch;
}

void count(void)
{
	int n, i;
	printf("Count how far? Enter an integer:\n");
	n = get_int();
	for(i = 1; i <= n; i++)
		printf("%d\n",i);
	while(getchar() != '\n')
		continue;
}

int get_int()
{
	int input_number;
	char invalid_ch;
	while(scanf("%d",&input_number) != 1)
	{
		while((invalid_ch = getchar()) != '\n')
			putchar(invalid_ch);
		printf(" is not an integer\n");
		printf("Please enter an integer such as 15, 3, -10.\n");
	}
	return input_number;
}