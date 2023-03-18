/*
op_client.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4
#define RLT_SIZE 4

void error_handler(char *msg);

int main(int argc, char *argv[])
{
	int client_sock;
	int opnd_cnt;
	int i;
	int result;
	char opmsg[BUF_SIZE];
	struct sockaddr_in serv_addr;

	if(argc != 3)
	{
		printf("Usage : %s [ip] [port]", argv[0]);
		exit(1);
	}

	client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(client_sock == -1)
		error_handler("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	if(connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handler("connect() error");
	else
		puts("connected....");

	fputs("Operand count:", stdout);
	scanf("%d", &opnd_cnt);
	opmsg[0] = (char)opnd_cnt;

	for(i = 0; i < opnd_cnt; i++)
	{
		printf("Operand %d", i + 1);
		scanf("%d", (int *)opmsg[i * OPSZ + 1]);
	}

	fgetc(stdin);
	fputs("Operator", stdout);
	scanf("%c", &opmsg[opnd_cnt * OPSZ + 1]);
	write(client_sock, opmsg, opnd_cnt * OPSZ + 2);
	read(client_sock, &result, RLT_SIZE);

	printf("Operator result : %d\n", result);
	close(client_sock);
	return 0;
}

void error_handler(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}