/*
hello_server.c,wguo 2021/07/31
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handler(char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[] = "Hello World";

	if(argc!=2)
	{
		printf("Usage : %s <port>",argv[0]);
		exit(1);
	}

	/* 创建socket */
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	if(serv_sock == -1)
		error_handler("socket() error");

	/* 构建地址 */
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	/* 绑定地址 */
	if(bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1)
		error_handler("bind() error");

	/* 开启监听状态 */
	if(listen(serv_sock,5) == -1)
		error_handler("listen() error");

	/* 接受连接请求 */
	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_addr,&clnt_addr_size);
	if(clnt_sock == -1)
		error_handler("accept() error");
	write(clnt_sock,message,sizeof(message));
	close(clnt_sock);
	close(serv_sock);

	return 0;
}

void error_handler(char *message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
