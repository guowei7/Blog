/**
 * op_server.c 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4

void error_handling(char *message);
int calculate(int optnum, int opnds[], char operator);

int main(int argc, char *argv[])
{
	int serv_sock;
	int client_sock;
	int i = 0;
	int opnd_cnt = 0;
	int recv_len = 0;
	int recv_cnt = 0;
	char opinfo[BUF_SIZE];
	int result = 0;
	socklen_t client_addr_sz;
	struct sockaddr_in serv_addr, client_addr;

	if(argc != 2)
	{
		printf("Usage : %s [port]\r\n", argv[0]);
		exit(1);
	}

	/*
	创建服务端fd
	 */	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	/*
	绑定服务端sock的ip和端口，通过sockaddr_in强转sockaddr
	 */
	if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr) == -1))
		error_handling("bind() error");

	/*
	服务端sock用于监听，监听队列大小为5
	 */
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	client_addr_sz = sizeof(client_addr);
	for(i = 0; i < 5; i++)
	{
		opnd_cnt = 0;
		client_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &client_addr_sz);

		/* 读取第一个字节，获取操作数个数 */
		read(client_sock, &opnd_cnt, 1);

		/*
		循环读取opnd_cnt个操作数，并读取最后一个字节的操作符
		 */
		recv_len = 0;
		while(opnd_cnt * OPSZ + 1 > recv_len)
		{
			recv_cnt = read(client_sock, &opinfo[recv_len], BUF_SIZE - 1);
			recv_len += recv_cnt;
		}

		result = calculate(opnd_cnt, (int *)opinfo, opinfo[recv_len - 1]);
		write(client_sock, (char *)&result, sizeof(result));
		close(client_sock);
	}	
	close(serv_sock);
	return 0;
}

int calculate(int optnum, int opnds[], char operator)
{
	int result = opnds[0];
	int i = 0;

	switch(operator)
	{
		case '+':
		{
			for(i = 1; i < optnum; i++)
				result += opnds[i];			
			break;
		}
		case '-':
		{
			for(i = 1; i < optnum; i++)
				result -= opnds[i];
			break;
		}
		case '*':
		{
			for(i = 1; i < optnum; i++)
				result *= opnds[i];
			break;
		}
		default:
			break;
	}
	return result;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}