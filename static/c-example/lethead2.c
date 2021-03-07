#include <stdio.h>
#include <string.h>
#define NAME "GIGATHINK, INC."
#define ADDRESS "101 Megabuck Plaza"
#define PLACE "Megapolis, CA 94904"
#define WIDTH 40
#define SPACE ' '

void show_n_char(char ch, int num);

int main(void)
{
    int spaces;
    show_n_char('*', WIDTH);
    putchar('\n');
    show_n_char(SPACE, 12);
    printf("%s\n", NAME);
    spaces = (WIDTH - strlen(ADDRESS)) / 2;
    show_n_char(SPACE,spaces);
    printf("%s\n", ADDRESS);
    spaces = (WIDTH-strlen(PLACE))/2;
    show_n_char(SPACE,spaces);
    printf("%s\n", PLACE);
    show_n_char('*',WIDTH);
    putchar('\n');
    return 0;
}

/**
 * fn       show_n_char
 * authoer  wguo
 * date     2021-01-13
 * brief    printf num characters ch
 * relates  no
 * remarks  no
 * @param ch  printed character
 * @param num the num of printed character 
 */
void show_n_char(char ch,int num)
{
	int count;
	for (int count = 1; count <= num; count++)
	{
		putchar(ch);
	}
}