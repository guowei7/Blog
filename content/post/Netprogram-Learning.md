---
title: "Netprogram Learning"
date: 2021-07-28T22:06:50+08:00
Description: ""
Tags: [
    "网络编程"
    ]
Categories: [
    "计算机网络"
    ]
DisableComments: false
---

主要为网络编程的相关学习
<!--more-->
### 1. 理解网络编程和套接字   
1. 理解网络编程和套接字  

+ 网络编程和套接字概要  
    操作系统会提供套接字(socket)来完成数据的传输，因此网络编程也可以被称为套接字编程。  
+ 构建接电话套接字  
    * 调用socket函数(安装电话机)  
    创建套接字并返回文件描述符

    ```C
    #include <sys/socket.h>  
    /* 成功时返回文件描述符，失败时返回-1 */
    int socket(int domain,int type,int protocol);
    ```

    * 调用bind函数(分配电话号码)  
    给创建好的套接字分配地址信息(ip地址和端口号)  
    
    ```C
    #include <sys/socket.h>  
    /* 成功时返回0，失败时返回-1 */
    int bind(int socketfd, struct sockadr *myaddr,socklen_t addrLen); 
    ```

    * 调用listen函数(连接电话线)  
    将套接字转换成可接受连接状态  

    ```C
    #include <sys/socket.h>  
    /* 成功时返回0，失败时返回-1 */
    int listen(int sockfd,int backlog);
    ```

    * 调用accept函数(拿起话筒)  
    接受对端的连接请求  

    ```C
    #include <sys/socket.h>
    /* 成功时返回文件描述符，失败时返回-1 */
    int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen)
    ```

+ 编写服务端  
    服务器端是能够受理连接请求的程序。服务端收到请求后向请求者返回"Hello world!"回复。  
    [hello_server.c](../../c-example/socket/Chap1/hello_server.c)  

+ 构建打电话套接字  
    服务端创建的套接字又称为服务端套接字或监听套接字。而用于请求连接的客户端套接字如下：  
    ```C
    #include <sys/socket.h>
    /* 成功时返回0，失败时返回-1 */
    int connect(int sockfd,struct sockaddr *serv_addr,socklen_t addrLen);
    ```

    客户端程序只有"调用socket函数创建套接字"和"调用connect函数向服务器端发送连接请求"这两个步骤。  

    [hello_client.c](../../c-example/socket/Chap1/hello_client.c)  

2. 基于Linux的文件操作  
对于Linux而言，socket操作与文件操作没有区别，因而有必要详细了解文件。因此在网络数据传输过程中自然可以使用文件I/O的相关函数。Windows则与Linux不同，是要区分socket和文件的。因此在Windows中需要调用特殊的数据传输相关函数。  

+ 底层文件访问(Low_Level File Access)和文件描述符(File Descriptor)  
    文件描述符时系统分配给文件或套接字的整数，实际上C语言的标准输入即输出和标准错误输出在Linux中也被分配了文件描述符。  

    |文件描述符|对象|
    |:--:|:--:|
    |0|标准输入|
    |1|标准输出|
    |2|标准错误|

    文件和套接字一般经过创建才会被分配文件描述符。而上述的三种对象未经过特殊的创建过程，程序开始运行后也会被自动分配文件描述符。  
+ 打开文件  
    
    ```C
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>

    /* 成功时返回文件描述符，失败时返回-1 */
    int open(const char *path,int flag);
    ```

+ 关闭文件  

    ```C
    #include <unistd.h>
    /* 成功时返回0，失败时返回-1 */
    int close(int fd);
    ```

    此函数不仅可以关闭文件，还可以关闭套接字。因为Linux操作系统不区分文件与套接字。  
+ 将数据写入文件  
    
    ```C
    #include <unistd.h>
    /* 成功时返回写入的字节数，失败时返回-1
    *  fd 数据传输对象的文件描述符
    *  buf 保存要传输数据的缓冲地址值
    *  nbytes  要传输数据的字节数。
   */
    ssize_t write(int fd, const void *buf,size_t nbytes);
    ```

    [low_open.c](../../c-example/socket/Chap1/low_open.c)  

+ 读取文件中的数据  
    
    ```C
    #include <unistd.h>
    /* 成功时返回接受的字节数(遇到文件结尾则返回0),失败返回-1
    *  fd 数据接受对象的文件描述符
    *  buf 要保存接收数据的缓冲地址值
    *  nbytes 要接收数据的最大字节数
    */
    ssize_t read(int fd,void *buf,size_t nbytes);
    ```
    [low_read.c](../../c-example/socket/Chap1/low_read.c)  

+ 文件描述符与套接字  

### 2. 套接字类型与协议设置  
1. 套接字协议及其数据传输特性  
+ 关于协议  
    协议就是为了完成数据交换而定好的约定。  

+ 创建套接字  
    创建套接字所用的socket函数主要包含三个参数  

    * domain 套接字中使用的协议族信息  
    * type 套接字数据传输类型信息  
    * protocol 计算机间通信中使用的协议信息  

+ 协议族  
    
    |名称|协议族|
    |:--:|:--:|
    |PF_INET|IPv4互联网协议族|
    |PF_INET|IPv6互联网协议族|
    |PF_LOCAL|本地通信的UNIX协议族|
    |PF_PACKET|底层套接字的协议族|
    |PF_IPX|IPX Novell协议族|

+ 套接字类型  
    套接字类型指的是套接字的数据传输方式，通过socket函数的第二个参数传递，只有这样才能决定创建的套接字的数据传输方式。具有代表性的数据传输方式有下列两种。  
    * 面向连接的套接字(SOCK_STREAM)  
        主要的特点如下：  
        - 传输过程中数据不会消失  
        - 按序传输数据  
        - 传输的数据不存在数据边界  

        面向连接的套接字只能与另外一个同样特性的套接字连接。"可靠的、按序传递的、基于字节的面向连接的数据传输方式的套接字"  

    * 面向消息的套接字(SOCK_DGRAM)  
        主要的特点如下：  
        - 强调快速传输而非传输顺序  
        - 传输的数据可能丢失也可能损毁  
        - 传输的数据有数据边界  
        - 限制每次传输的数据大小  

        面向消息的套接字比面向连接的套接字具有更快的传输速度，但无法避免数据丢失或损毁。另外每次传输的数据大小具有一定限制，并存在数据边界。存在数据边界意味着接受数据的次数应和传输次数相同。"不可靠的、不按序传递的、以数据的高速传输为目的的套接字"  

+ 协议的最终选择  
    传递前两个参数即可创建所需套接字。所以大部分情况下可以向第三个参数传递0，除非遇到以下这种情况。  
    同一协议族中存在多个数据传输方式相同的协议。  
    数据传输方式相同，但协议不同。此时需要通过第三个参数具体指定协议信息。  
    IPv4协议族中面向连接的套接字只有TCP套接字。  

    >int tcp_socket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    IPv4协议族中面向消息的套接字只有UDP套接字。  

    >int udp_socket = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);  

### 3. 地址族与数据序列  
+ 分配给套接字的IP地址与端口号  
    IP地址是为了收发网络数据而分配给计算机的值。端口号并非赋予计算机的值，而是为区分程序中创建的套接字而分配给套接字的序号。  

    1. 网络地址  
    为使计算机连接到网络并收发数据，必需向其分配IP地址。IP地址分为两类。  
    - IPv4 4字节地址族  
    - IPv6 16字节地址族  
    IP地址分为网络地址和主机地址，网络地址是为区分网络而设置的一部分IP地址，当数据传输到所在网络后，在通过主机地址将数据传给目标计算机。  

    2. 网络地址分类与主机地址边界  
    只需通过IP地址的第一个字节即可判断网络地址占用的字节数。  
    - A类地址的首位以0开始  
    - B类地址的前两位以10开始  
    - C类地址的前两位以110开始  

    3. 用于区分套接字的端口号  
    IP用于区分计算机，只要有IP地址就能向目标主机传输数据，但仅凭这些无法传输给最终的应用程序。  
    计算机中一般配有NIC(网络接口卡)数据传输设备，通过NIC向计算机内部传输数据时会用到IP。操作系统负责把传递到内部的数据适当分配给套接字，这时就要利用端口号，也就是说，通过NIC接收的数据内有端口号，操作系统正是参考此端口号把数据传输给相应端口的套接字。  
    端口号就是在同一操作系统内为区分不同套接字而设置的，因此无法将一个端口号分配给不同套接字。另外，端口号由16位构成，可分配的端口号范围是0-65535。但0-1023是知名端口(Well-known PORT)，一般分配给特定应用程序，所以应当分配此范围之外的值。另外，虽然端口号不能重复，但TCP套接字和UDP套接字不会共用端口号，所以允许重复。  

+ 地址信息的表示  
    应用程序中使用的IP地址和端口号以结构体的形式给出定义。  

    1. 表示IPv4地址的结构体  

    ```C
    struct sockaddr_in
    {
        sa_family_t sin_family;  //地址族(Address Family)
        uint16_t sin_port;  //16位TCP/UDP端口号
        struct in_addr sin_addr;  //32位IP地址
        char  sin_zero[8];  //不使用
    };
    ```
    
    该结构体中另一个结构体in_addr定义如下,用来存放32位IP地址  

    ```C
    struct in_addr
    {
        In_addr_t  s_addr;  //32位IPv4地址
    };
    ```

    POSIX(Portable Operating System Interface,可移植操作系统接口),POSIX是位UNIX系列操作系统设立的标准，它定义了一些其他数据类型。  

    |数据类型名称|数据类型说明|声明的头文件|
    |:--:|:--:|:--:|
    |int8_t|signed 8-bit int|sys/types.h|
    |uint8_t|unsigned 8-bit int|
    |int16_t|signed 16-bit int
    |uint16_t|unsigned 16-bit int|
    |int32_t|signed 32-bit int|
    |uint32_t|unsigned 32-bit int|
    |sa_family_t|地址族|sys/socket.h|
    |socklen_t|长度|
    |in_addr_t|IP地址，声明为uint32_t|netinet/in.h|
    |in_port_t|端口号，声明为uint16_t|

    2. 结构体sockaddr_in的成员分析  

    + 成员sin_family  
        每种协议族适用的地址族均不同。比如，IPv4使用4字节地址族(AF_INET),IPv6使用16字节地址族(AF_INET6)  
    + 成员sin_port  
        该成员保存16位端口号，重点在于，它以网络字节序保存。  
    + 成员sin_addr  
        该成员保存32位IP地址信息，且也以网络字节序保存。理解成uint32_t即可。  
    + 成员sin_zero  
        无特殊含义。只是为使结构体sockaddr_in的大小与sockaddr结构体保持一致而插入的成员。必需填充为0，否则无法得到想要的结果。  

        ```C
        struct sockaddr_in serv_addr;
        if(bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1)

        struct sockaddr
        {
            sa_family_t sin_family;  //地址族
            char  sa_data[14];  //地址信息
        };
        ```

       sockaddr中sa_data保存的地址信息中需要包含IP地址和端口号，剩余部分填充0，这是bind函数要求的。这样直接使用十分麻烦，因此有了sockaddr_in，这样更加方便，使用的时候直接转换一下类型即可。

+ 网络字节序与地址变换  
    不同CPU中，4字节整型值1在内存空间的保存方式是不同的。4字节整型值1可用2进制表示如下 

    >00 00 00 01  

    有些CPU以这种顺序保存到内存，另外一些CPU则以倒序保存  

    >01 00 00 00 

1. 字节序与网络字节序  
    CPU向内存保存数据的方式有2种，这意味着CPU解析数据的方式也分为2种  

    * 大端序(Big Endian):  高位字节存放到低位地址  
    * 小端序(Little Endian): 高位字节存放到高位地址  

    代表CPU数据保存方式的主机字节序在不同CPU中也各不相同。目前主流的Intel系列CPU以小端序方式保存数据。  
    在通过网络传输数据时约定统一方式，这种约定称为网络字节序，统一为大端序。即，先把数据组织转化成大端序格式再进行网络传输。  
2. 字节序转换  
    帮助转换字节序的函数  

    * unsigned short htons(unsigned short)  
    * unsigned short ntohs(unsigned short)  
    * unsigned long htonl(unsigned long)  
    * unsigned long ntohl(unsigned long)  

    有必要编写与大端序无关的统一代码。这样即使在大端序系统中，最好也经过主机字节序转换为网络字节序的过程。当然，此时，主机字节序与网络字节序相同，不会有任何变化  

    ```C
    #include <stdio.h>
    #include <arpa/inet.h>

    int main(int argc,char *argv[])  
    {
        unsigned short host_port = 0x1234;
        unsigned short_net_port;
        unsigned long host_addr =  0x12345678;
        unsigned long net_addr;

        net_port=htons(host_port);
        net_addr=htonl(host_addr);

        printf("Host ordered port:%x",host_port);
        printf("Network ordered port:%x",net_port);
        printf("Host ordered address:%x",host_addr);
        printf("Network ordered address:%x",net_addr);
    }
    
    ```

    除了向sockaddr_in结构体变量填充数据外，其他情况无需考虑字节序问题。  
+ 网络地址的初始化与分配  
1. 将字符串信息转换为网络字节序的整数型  
    sockaddr_in中保存地址信息的成员为32位整数型。而我们熟悉的IP地址表示是点分十进制表示法，而非整数型数据表示法。有个函数会帮我们将字符串形式的IP地址转换成32位整数型数据。此函数在转换类型的同时进行网络字节序转换。  

    ```C
    #include <arpa/inet.h>

    /* 成功时返回32位大端序整数型值，失败返回INADDR_NONE*/
    int_addr_t inet_addr(const char *string);

    /* 成功时返回1，失败时返回0 */
    int inet_aton(const char *string,struct int_addr *addr);
    ```
   
    inet_addr函数不仅可以把IP地址转成32位整数型，而且可以检测无效的IP地址。  
    inet_aton函数与inet_addr函数在功能上完全相同，也将字符串形式IP地址转换位32位网络字节序整数并返回。只不过该函数利用了in_addr结构体，且使用频率更高。  

    介绍一个与inet_aton函数正好相反的函数，此函数可以把网络字节序整数型IP地址转换成我们熟悉的字符串形式。  

    ```C
    #include <arpa/inet.h>

    /* 成功时返回转换的字符串地址值，失败时返回-1 */
    char * inet_ntoa(struct in_addr addr);
    ```

2. 网络地址初始化  
    现介绍套接字创建过程中常见的网络地址信息初始化方法。  

    ```C
    struct sokaddr_in addr;
    char * serv_ip = "211.217.168.13";  
    char * serv_port = "9190";
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serv_ip);
    addr.sin_port = htons(atoi(serv_port));
    ```

3. 客户端地址信息初始化  
    客户端声明sockaddr_in结构体，并初始化为要与之连接的服务器端套接字的IP和端口号，然后调用connect函数。  
4. INADDR_ANY  
    每次创建服务器端套接字都要输入IP地址会有些繁琐，此时可以如下初始化  

    ```C
    struct sockaddr_in addr;
    char * serv_port = "9190";
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htol(INADDR_ANY);
    addr.sin_port = htons(atoi(serv_port));
    ```

    利用常数INADDR_ANY分配服务器端的IP地址。若采用这种方式，则可自动获取运行服务器端的计算机IP地址，不必亲自输入。若同一计算机中已分配多个IP地址，则只要端口号一致，就可以从不同IP地址接受数据。  
5. hello_server.c和hello_client.c运行过程  
6. 向套接字分配网络地址  
    sockaddr_in结构体初始化后，就应该分配给套接字，bind函数就负责这项此操作  

    ```C
    #include <sys/socket.h>

    /* 成功时返回0，失败时返回-1 */
    int bind(int sockfd,struct sockaddr *myaddr,socklen_t addrlen);
    ```
    
