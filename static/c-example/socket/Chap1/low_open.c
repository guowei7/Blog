/*
low_open.c,wguo 2021/08/01
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handler(char *message);

int main(void)
{
	int fd;
	char buf[] = "Let's go!\n";

	fd = open("data.txt",O_CREAT|O_WRONLY|O_TRUNC);
	if(fd == -1)
		error_handler("open() error");
	printf("file descriptor : %d\n",fd);

	if(write(fd,buf,sizeof(buf)) == -1)
		error_handler("write() error");
	close(fd);
	return 0;
}

void error_handler(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
