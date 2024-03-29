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
    文件描述符是系统分配给文件或套接字的整数，实际上C语言的标准输入即输出和标准错误输出在Linux中也被分配了文件描述符。  

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

    套接字实际采用的最终协议信息是通过socket函数的第三个参数传递的。在指定的协议族范围内通过第一个参数决定第三个参数。  

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
1. 分配给套接字的IP地址与端口号  
    IP地址是为了收发网络数据而分配给计算机的值。端口号并非赋予计算机的值，而是为区分程序中创建的套接字而分配给套接字的序号。  

    1. 网络地址  
    为使计算机连接到网络并收发数据，必需向其分配IP地址。IP地址分为两类。  
    - IPv4 4字节地址族  
    - IPv6 16字节地址族  
    IP地址分为网络地址和主机地址，网络地址是为区分网络而设置的一部分IP地址，当数据传输到所在网络后，再通过主机地址将数据传给目标计算机。  

    2. 网络地址分类与主机地址边界  
    只需通过IP地址的第一个字节即可判断网络地址占用的字节数。  
    - A类地址的首位以0开始  
    - B类地址的前两位以10开始  
    - C类地址的前两位以110开始  

    3. 用于区分套接字的端口号  
    IP用于区分计算机，只要有IP地址就能向目标主机传输数据，但仅凭这些无法传输给最终的应用程序。  
    计算机中一般配有NIC(网络接口卡)数据传输设备，通过NIC向计算机内部传输数据时会用到IP。操作系统负责把传递到内部的数据适当分配给套接字，这时就要利用端口号，也就是说，通过NIC接收的数据内有端口号，操作系统正是参考此端口号把数据传输给相应端口的套接字。  
    端口号就是在同一操作系统内为区分不同套接字而设置的，因此无法将一个端口号分配给不同套接字。另外，端口号由16位构成，可分配的端口号范围是0-65535。但0-1023是知名端口(Well-known PORT)，一般分配给特定应用程序，所以应当分配此范围之外的值。另外，虽然端口号不能重复，但TCP套接字和UDP套接字不会共用端口号，所以允许重复。  

2. 地址信息的表示  
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

3. 网络字节序与地址变换  
    不同CPU中，4字节整型值1在内存空间的保存方式是不同的。4字节整型值1可用2进制表示如下 

    >00 00 00 01  

    有些CPU以这种顺序保存到内存，另外一些CPU则以倒序保存  

    >01 00 00 00 

+ 字节序与网络字节序  
    CPU向内存保存数据的方式有2种，这意味着CPU解析数据的方式也分为2种  

    * 大端序(Big Endian):  高位字节存放到低位地址  
    * 小端序(Little Endian): 高位字节存放到高位地址  

    代表CPU数据保存方式的主机字节序在不同CPU中也各不相同。目前主流的Intel系列CPU以小端序方式保存数据。  
    在通过网络传输数据时约定统一方式，这种约定称为网络字节序，统一为大端序。即，先把数据组织转化成大端序格式再进行网络传输。  

+ 字节序转换  
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

4. 网络地址的初始化与分配  

+ 将字符串信息转换为网络字节序的整数型  
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

+ 网络地址初始化  
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

+ 客户端地址信息初始化  
    客户端声明sockaddr_in结构体，并初始化为要与之连接的服务器端套接字的IP和端口号，然后调用connect函数。  

+ INADDR_ANY  
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

+ hello_server.c和hello_client.c运行过程  

+ 向套接字分配网络地址  
    sockaddr_in结构体初始化后，就应该分配给套接字，bind函数就负责这项此操作  

    ```C
    #include <sys/socket.h>

    /* 成功时返回0，失败时返回-1 */
    int bind(int sockfd,struct sockaddr *myaddr,socklen_t addrlen);
    ```

    如果此函数调用成功，则将第二个参数指定的地址信息分配给第一个参数中的相应套接字。  
    常见服务端套接字初始化过程如下:  

    ```C
    #include <sys/socket.h>
    #include <arpa/net.h>

    int serv_sock;
    struct sockaddr_in serv_addr;
    char * serv_port = "9190";

    /* 创建服务端套接字(监听套接字) */
    serv_port = sockett(PF_INET, SOCK_STREAM, 0);

    /* 地址信息初始化 */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(serv_port));

    /* 分配地址信息 */
    bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    ```
    
### 4. 基于TCP的服务端/客户端(1)
1. 理解TCP和UDP  
根据数据传输方式的不同，基于网络协议的套接字一般分为TCP套接字和UDP套接字。因为TCP套接字是面向连接的，因此又称基于流(stream)的套接字。  
TCP是Transmission Control Protocol(传输控制协议)的简写，意为“对数据传输过程的控制”。因此，学习控制方法及范围有助于正确理解TCP套接字。  

+ TCP/IP协议栈  

    >应用层  
    >TCP层、UDP层  
    >IP层  
    >链路层  

    可以看出，TCP/IP协议栈共分4层，可以理解为数据收发分成了4个层次化过程。也就是说，面对"基于互联网的有效数据传输"的命题，并非通过1个庞大协议解决问题，而是化整为零，通过层次化方案——TCP/IP协议栈解决。通过TCP套接字收发数据时需要借助这4层。  
    各层可能通过操作系统等软件实现，也可能通过类似NIC的硬件设备实现。  

+ TCP/IP协议的诞生背景  
    "通过因特网完成有效数据传输"这个问题并非仅凭软件就能解决。编写软件前需要构建硬件系统，在此基础上需要通过软件实现各种算法。把该问题按照不同领域划分成小问题后，出现多种协议，它们通过层级结构建立了紧密联系。  
    开放式系统(Open System):以多个标准为依据设计的系统称为开放式系统，我们现在学习的TCP/IP协议栈也属于其中之一。其优点在于统一化，比如路由器用来完成IP层交互任务。某公司原来使用A公司的路由器，现要将其替换成B公司的，是否可行？这并非难事，并不一定要换成同一公司的同一型号路由器，因为所有生产商都会按照IP层标准制造。  
    标准的存在意味着高速的技术发展，这也是开放式系统设计最大的原因所在。也就是说，标准对于技术发展起着举足轻重的作用。  

+  链路层  
    链路层是物理链接领域标准化的结果，也是最基本的领域，专门定义LAN、WAN、MAN等网络标准。若两台主机通过网络进行数据交换，则需要物理连接，链路层就负责这些标准。  

+ IP层  
    为了在复杂的网络中传输数据，首先需要考虑路径的选择。向目标传输数据需要经过那条路径？解决此问题就是IP层，该层使用的协议就是IP。  
    IP本身是面向消息的、不可靠的协议。每次传输数据时会帮我们选择路径，但并不一致。如果传输中发生路径错误，则选择其他路径；但如果发生数据丢失或错误，则无法解决。  

+ TCP/UDP层  
    IP层解决数据传输中的路径选择问题，只需照此路径传输数据即可。TCP和UDP层以IP层提供的路径信息为基础完成实际的数据传输，故该层又称传输层(Transport)。TCP可以保证可靠的数据传输，但它发送数据时以IP层为基础。  
    IP层只关注一个数据包的传输过程。因此，即使传输多个数据包，每个数据包也是由IP层实际传输的，也就是说传输顺序及传输本身是不可靠的。若只利用IP层传输数据，则有可能导致后传输的数据包B比先传输的数据包A提早到达。另外，传输的数据包A、B、C中有可能只收到A和C，甚至收到的C可能已损毁。反之，若添加TCP协议则按照如下对话方式进行数据交换。  

    >A: "正确收到第二个数据包！"  
    >B:"嗯，知道了。"  
    >A:"正确收到第三个数据包！"  
    >B:"可我已发送第四个数据包了啊！哦，你没收到第四个数据包吧？我给你重传！"  

    这就是TCP的作用。如果数据交换过程中可以确认对方已收到数据，并重传丢失的数据，那么即便IP层不保证数据传输，这类通信也是可靠的。  

+ 应用层  
    上述内容是套接字通信过程中自动处理的。选择数据传输路径、数据确认过程都被隐藏到套接字内部。而与其说是"隐藏"，倒不如"使程序员从这些细节中解放出来"的表达更为准确。  
    总之，提供的工具就是套接字，大家只需要利用套接字编出程序即可。编写软件的过程中，需要根据程序特点决定服务器端和客户端之间的数据传输规则，这便是应用层协议。网络编程的大部分内容就是设计并实现应用层协议。  

2. 实现基于TCP的服务器端/客户端  
    + TCP服务器端的默认函数调用顺序  
        TCP服务器端默认的函数调用顺序如下:  

        >scoket()  创建套接字  
        >bind()  分配套接字地址  
        >listen()  等待连接请求状态  
        >accept()  允许连接  
        >read()/write()  数据交换  
        >close()  断开连接  

    + 进入等待连接请求状态  
        调用bind函数给套接字分配地址后，要通过listen函数进入等待连接请求状态。只有调用了listen函数，客户端才能进入可发出连接请求的状态。这样，客户端才能调用connect函数。  

        ```C
        #include <sys/socket.h>

        int listen(int sock, int backlog);
        /*sock:希望进入等待连接请求状态的套接字文件描述符，传递的描述符套接字参数成为服务器端套接字。  
        *backlog:连接请求等待队列的长度，表示最多使5个连接请求进入队列
        */
        ```

        等待连接请求状态:客户端请求连接时，受理连接前一直使请求处于等待状态。客户端连接请求本身也是从网络中接收到的一种数据，而要想接收就需要套接字,此任务就由服务器端套接字完成。  
        连接请求等待队列:连接请求的等候室。  

    + 受理客户端连接请求  
        调用listen函数后，若有新的连接请求，则应按序受理。受理请求意味着进入可接收数据的状态,此时需要套接字来处理数据的接收，而listen函数创建的套接字已经用来接收连接请求。故需要一个套接字，但不需要通过socket函数来创建，下面的accept函数将自动创建套接字，并连接到发起请求的客户端。  

        ```C
        #include <sys/socket.h>

        /* 成功时返回创建的套接字文件描述符，失败时返回-1 */
        int accept(int sock, struct sockaddr *addr, socklen_t *addrlen);
        /* sock:服务器套接字的文件描述符。
        * addr:保存发起连接请求的客户端地址信息的变量地址值，调用函数后向传递来的地址变量参数填充客户端地址信息。
        * addrlen:第二个参数addr结构体的长度，但是存有长度的变量地址。函数调用完成后，该变量即被填入客户端地址长度。 
        ```

        accept函数受理连接请求等待队列中待处理的客户端连接请求。函数调用成功时，accept函数内部将产生用于数据I/O的套接字，并返回其文件描述符。套接字是自动创建的，并自动与发起连接请求的客户端建立连接。  

    + 回顾Hello wordl的服务器端  
        只有调用listen函数进入等待连接请求状态后的套接字才是服务器端套接字。调用accept函数从队头取1个连接请求与客户端建立连接，并返回创建的套接字文件描述符。调用accept函数时若等待队列为空，则accept函数不会返回，直到队列中出现新的客户端连接。  

    + TCP客户端的默认函数调用顺序  
        客户端默认函数调用顺序如下:  

        >socket()  创建套接字
        >connect()  请求连接
        >read()/write()  交换数据
        >close()  断开连接

       "请求连接"是创建客户端套接字后向服务器端发起的连接请求。服务器端调用listen函数后创建连接请求等待队列，之后客户端即可请求连接。发起连接请求主要靠connect函数。  
       
       ```C 
       #include <sys/socket.h>

       int connect(int sock, struct sockaddr * servaddr, socklen_t addrlen);
       /*sock:客户端套接字文件描述符
       *servaddr:保存目标服务器端地址信息的变量地址值
       *addrlen:以字节为单位传递已传递给第二个结构体参数servaddr的地址变量长度
       */
       ```

       客户端调用connect函数后，发生以下情况之一才会返回(完成函数调用)  

       * 服务器端接收连接请求  
       * 发生断网等异常情况而中断连接请求  
       
       需要注意，所谓的"接收连接"并不意味着服务器端调用accept函数，其实是服务器端把连接请求信息记录到等待队列。因此connect函数返回后并不立即进行数据交换。***客户端的IP地址和端口在调用connect函数时自动分配(由操作系统内核分配，IP为主机IP，端口随机)，无需调用标记的bind函数进行分配***  

    + 回顾Hello world客户端  
        结构体变量serv_addr中初始化IP和端口信息。初始化值为目标服务器端套接字的IP和端口信息。调用connect函数向服务器端发送连接请求。  

    + 基于TCP的服务器端/客户端函数调用关系  
        TCP服务器端/客户端并非相互独立，之间的交互过程如下:  

        >socket()  
        >bind()-----------------socket()  
        >listen()<--------------connect()  
        >accept()<--------------------|  
        >read()/write()<------->read()/write()  
        >close()<--------------->close()  
    
        服务器端创建套接字后连续调用bind、listen函数进入等待状态，客户端通过调用connect函数发起连接请求。客户端只能等到服务器端调用listen函数后才能调connect函数。同时要清楚，客户端调用connect函数前，服务器端有可能率先调用accept函数。当然，此时服务器端在调用accept函数时进入阻塞(blocking)状态，直到客户端调用connect函数为止。  
        
3. 实现迭代服务器端/客户端  
    本节编写回声(echo)服务器/客户端，服务器端将客户端传输的字符串数据原封不动地传回客户端，就像回声一样。  

    + 实现迭代服务器端  
        之前讨论地Hello world服务器端处理完一个客户端连接请求即退出，连接请求等待队列实际没有太大地意义。设置好等待队列的大小后，应向所有客户端提供服务。如果想继续受理后续的客户端连接请求，最简单的办法就是插入循环语句反复调用accept函数。  
        调用accept函数后，紧接着调用I/O相关的read、write函数，然后调用close函数。这并非针对服务端套接字，而是针对accept函数调用时创建的客户端套接字。  
        调用close函数意味着结束了针对某一客户端的服务。此时如果还想服务于其他客户端，就要重新调用accept函数。  
        同一时刻确实只能服务于一个客户端。等了解进程和线程后，就可以编写同时服务多个客户端的服务器端了。  
        
    + 迭代回声服务器端/客户端  
        即使服务器端以迭代方式运转，客户端代码亦无太大区别。接下来创建迭代回声服务器端及与其配套的回声客户端。首先整理程序的基本运行方式。  
        1. 服务器端在同一时刻只与一个客户端相连，并提供回声服务。  
        2. 服务器端依次向5个客户端提供服务并退出。  
        3. 客户端接收用户输入的字符串并发送到服务器端。  
        4. 服务器端将接收的字符串数据传回客户端，及"回声"。  
        5. 服务器端与客户端之间的字符串回声一直执行到客户端输入Q为止。  
        
        [echo_server.c](../../c-example/socket/Chap4/echo_server.c)  
        [echo_client.c](../../c-example/socket/Chap4/echo_client.c)  

    + 回声客户端存在的问题  
        下面是echo_client.c的第45-48行代码。  
        ``` c
        write(sock, message, strlen(message));
        str_len = read(sock, message, BUF_SIZE - 1);
        message[str_len] = 0;
        pritnf("message from serv: %s",message);
        ```
        以上代码有个错误假设:

        >"每次调用read、write函数时都会以字符串为单位执行实际的I/O操作"  

        当然，每次调用write函数都会传递1个字符串，因此这种假设在某种程度上也算合理。但“TCP不存在数据边界”。因此多次调用write函数传递的字符串有可能一次性传递到服务器端。此时客户端有可能从服务器端收到多个字符串，这不是我们希望看到的结果。还需要考虑服务器端的如下情况:  

        >"字符串太长，需要分2个数据包发送"  
        
        服务器端希望通过调用1次write函数传输数据，但如果数据太大，操作系统就有可能把数据分成多个数据包发送到客户端。另外，在此过程中，客户端有可能在尚未收到全部数据包时就调用read函数。  

### 5. 基于TCP的服务端/客户端(2)
1. 回声客户端的完美实现  
    
   + 回声服务器端没有问题，只有回声客户端有问题  
       问题不在服务器端，而在客户端。服务器端的I/O代码如下：

       ``` c
       while((strlen = read(clnt_sock, message, BUF_SIZE)) != 0)
        write(clnt_sock, message, str_len);
       ```

       接着回顾回声客户端代码如下:

       ``` c
       write(sock, message, strlen(message));
       str_len = read(sock, message, BUF_SIZE - 1);
       ```

       二者都在循环调用read或write函数。实际上之前的回声客户端将100%接收自己传输的数据，只不过接收数据时的单位有些问题。扩展客户端代码回顾范围，代码如下:

       ``` c
       while(1)
       {
        fputs("Input mesage(Q to quit):", stdout);
        fgets(message, BUF_SIZE, stdin);
        ...
        write(sock, message, strlen(message));
        str_len = read(sock, message, BUF_SIZE -1);
        message[str_len] = 0;
        printf("Message from server:%s", message);
       }
       ```

       回声客户端传输的时字符串，而且是通过调用write函数一次性发送的。之后还调用一次read函数，期待着接收自己传输的字符串。  
       的确，过一段时间后即可接收，但需要等多久，理想的客户端应在收到字符串数据时立即读取并输出。  

   + 回声客户端问题解决  
       若之前传输了20字节长的字符串，则在接收时循环调用read函数读取20个字节即可。  

       [echo_client2.c](../../c-example/socket/Chap4/echo_client2.c)  

   + 如果问题不在于回声客户端：定义应用层协议  
       回声客户端可以提前知道接收的数据长度，但我们应该意识到，更多情况下这不太可能。既然如此，若无法预知接收数据长度时应如何收发数据？此时需要的就是应用层协议的定义。之前的回声服务器/客户端定义了如下协议。  

       >"收到Q就立即终止连接"  
       
       同样，收发数据过程中也需要定好规则(协议)以表示数据的边界，或提前告知收发数据的大小。服务器端/客户端实现过程中逐步定义的这些规则集合就是应用层协议。可以看出，应用层协议并不是高深莫测的存在，只不过是为特定程序的实现而制定的规则。  
       下面编写程序以体验应用层协议的定义过程。该程序中，服务器端从客户端获得多个数字和运算符信息。服务器端收到数字后对其进行加减乘运算，然后把结果传回客户端。例如，向服务器端传递3、5、9的同时请求加法运算，则客户端收到3+5+9的运算结果；若请求做乘法运算，则客户端收到3 * 5 * 9的运算结果。而如果向服务器端传递4、3、2的同时要求做减法，则客户端将收到4-3-2的运算结果，即第一个参数成为被减数。  
       
   + 计算器服务端/客户端示例  
       [op_client.c](../../c-example/socket/Chap4/op_client.c)  
       [op_server.c](../../c-example/scoket/Chap4/op_server.c)  
   
2. TCP原理  
    + TCP套接字中的I/O缓冲  
        TCP套接字的数据收发无边界。服务器端即使调用1次write函数传输40字节的数据，客户端也有可能通过4次read函数调用每次读取10字节。但此处也有一些疑问，服务器端一次性传输了40字节，而客户端居然可以缓慢地分批接收。客户端接收10字节后，剩下地30字节在何处等候呢？是不是像飞机为等待着陆而在空中盘旋一样，剩下30字节也在网络中徘徊并等待接收呢？  
        实际上write函数调用后并非立即传输数据，read函数调用后也并非马上接收数据。更准确地说。write函数调用瞬间，数据将移至输出缓冲；read函数调用瞬间，从输入缓冲读取数据。这些I/O缓冲特性可整理如下:  
        
        * I/O缓冲在每个TCP套接字中单独存在  
        * I/O缓冲在创建套接字时自动生成  
        * 即使关闭套接字也会继续传递输出缓冲中遗留地数据  
        * 关闭套接字将丢失输入缓冲中地数据  

        >"客户端输入缓冲为50字节，而服务器端传输了100字节"  
        
        因为TCP会控制数据流。TCP中有滑动窗口(Sliding Window)协议，用对话方式呈现如下:  
        >套接字A:"你好，最多可以向我传递50字节"  
        >套接字B:"OK!"   
        >套接字A:"我腾出了20字节的空间，最多可以收70字节"  
        >套接字B:"OK!"  

        write函数和Windows的send函数并不会在完成向对方主机的数据传输时返回，而是在数据移到输出缓冲时。但TCP会保证对输出缓冲数据的传输，所以说write函数在数据传输完成时返回。  

    + TCP的内部工作原理1:与对方套接字的连接  
        TCP套接字从创建到消失所经过程分为如下3步。  

        * 与对方套接字建立连接  
        * 与对方套接字进行数据交换  
        * 断开与对方套接字的连接  
        
        首先讲解与对方套接字建立连接的过程。连接过程中套接字之间的对话如下:  

        >[Shake1]套接字A:"你好，套接字B。我这儿有数据要传给你，建立连接吧。"  
        >[Shake2]套接字B:"好的，我这边已就绪。"  
        >[Shake3]套接字A:"谢谢你受理我的请求。"  

        TCP在实际通信过程中也会经过3次对话过程，因此，该过程又称Three-way handshaking(三次握手)。  

        >主机A===============主机B  
        >|----seq:1000;ack----->|  
        >|<--seq:2000;ack:1001--|  
        >|--seq:1001;ack2001--->|  
        
        套接字是以全双工(Full-duplex)方式工作的。也就是说，它可以双向传递数据。因此，收发数据前需要做一些准备。首先，请求连接的主机A向主机B传递如下信息:  

        >[SYN] SEQ:1000, ACK:-  

        该消息中SEQ为1000，ACK为空，而SEQ为1000的含义为：  

        >现传递的数据包序号为1000，如果接收无误，请通知我向您传递1001号数据包  
        
        这是首次请求连接时使用的消息，又称SYN。SYN是Synchronization的简写，表示收发数据前传输的同步消息。接下来主机B向A传递如下消息：  

        >[SYN+ACK] SEQ:2000, ACK:1001  
        
        此时SEQ为2000，ACK为1001。对应的含义为:  

        >现传递的数据包序号为2000，如果接收无误，请通知我向您传递2001号数据包  
        >刚才传输的SEQ为1000的数据包接收无误，现在请传递SEQ为1001的数据包  

        对主机A首次传输的数据包的确认消息(ACK1001)和为主机B传输数据做准备的同步消息(SEQ2000)捆绑发送，因此，此种类型的消息又称为SYN+ACK。  
        收发数据前向数据包分配序号，并向对方通报此序号，这都是为防止数据丢失所做的准备。通过向数据包分配序号并确认，可以在数据丢失时马上查看并重传丢失的数据包。因此，TCP可以保证可靠的数据传输。最后观察主机A向主机B传输的消息:  

        >[ACK] SEQ：1001，ACK：2001  
        
        之前也讨论过，TCP连接过程中发送数据包时需分配序号。在之前的序号1000的基础上加1,也就是分配1001。此时该数据包传递如下消息:  

        >已正确收到传输的SEQ为2000的数据包，现在可以传输SEQ为2001的数据包  
        
    + TCP的内部工作原理2:与对方主机的数据交换  
        通过第一步三次握手过程完成了数据交换准备，下面就正式开始收发数据，其默认方式如下:  

        >主机A==============主机B  
        >|==seq:1200,100bytes=>|  
        >|<======ack:1301======|  
        >|==seq:1301,100bytes=>|  
        >|<======ack:1401======|  
        
        主机A分两次(分2个数据包)向主机B传递200字节的过程。首先主机A通过1个数据包发送100个字节的数据，数据包的seq为1200。主机B为了确认这一点，向主机A发送ACk1301消息。  
        此时的ACK号为1301而非1201，原因在于ACK号的增加为传输的字节数。假设每次ACK号不加传输的字节数，这样虽然可以确认数据包的传输，但无法明确100字节全都正确传递还是丢失了一部分，比如只传递了80个字节。因此按如下公式传递ACK消息：

        >ACK号 = SEQ号 + 传递的字节数 + 1

        与三次握手协议相同，最后加1是为了告知对方下次要传递的SEQ号。下面分析传输过程中数据包消失的情况:  

        >主机A===================主机B  
        >|====seq:1200,100bytes====>|  
        >|<=========ack:1301========|  
        >|====seq:1301,100bytes====>|  
        >|=====fail,timeout=========|  
        >|====seq:1301,100bytes====>|  
        >|<=========ack:1402========|  

        如上主机A通过seq1301数据包向主机B传递100字节数据。但中间发生了错误，主机B未收到。经过一段时间后，主机A仍未收到对于SEQ1301的ACK确认，因此试着重传该数据包。为了完成数据重传，TCP套接字启动计时器等待ACK应答。若相应计时器发生超时则重传。  
        
    + TCP的内部工作原理3:断开与套接字的连接  
        TCP套接字的结束过程也非常优雅。如果对方还有数据需要传输时直接断掉连接会出问题，所以断开连接时需要双方协商。断开连接时双方对话如下：  

        >套接字A: "我希望断开连接"
        >套接字B: "哦，是吗？请稍候。"
        >套接字B: "我也准备就绪，可以断开连接。"
        >套接字A: "好的，谢谢合作"
        
        先由套接字A向套接字B传递断开连接的消息，套接字B发出确认收到的消息，然后向套接字A传递可以断开连接的消息，套接字A同样发出确认消息，如图:  

        >主机A=======================主机B  
        >|=====fin seq:5000,ack:-======>|  
        >|<====ack seq:7500,ack:5001====|  
        >|<====fin seq:7501,ack:5001====|  
        >|=====ack seq:5001,ack:7502===>|  
        
        数据包内的FIN表示断开连接。也就是说，双方各发送1次FIN消息后断开连接。此过程经历4各阶段，因此又称为四次握手。  

### 6. 基于UDP的服务端/客户端
1. 理解UDP  
    + UDP套接字的特点  
        UDP提供的同样是不可靠的数据传输服务。如果只考虑可靠性，TCP的确比UDP好。但UDP在结构上比TCP更简洁。UDP不会发送类似ACK的应答消息，也不会像SEQ那样给数据包分配序号。因此，UDP的性能有时比TCP高出很多。编程中实现UDP也比TCP简单。另外，UDP的可靠性虽比不上TCP，但也不会像想象中那么频繁地发生数据损毁。因此，在更重视性能而非可靠性的情况下，UDP是一种很好的选择。  
        TCP在不可靠的IP层进行流控制，而UDP就缺少这种流控制机制。流控制是区分UDP和TCP的最重要的标志。但若从TCP中除去流控制，所剩内容也屈指可数。也就是说，TCP的生命在于流控制。  

    + UDP内部工作原理  
        与TCP不同，UDP不会进行流控制。IP的作用是让离开主机B的UDP数据包准确传递到主机A。但把UDP包最终交给主机A的某一UDP套接字的过程则是由UDP完成的。UDP最重要的作用就是根据端口号将传到主机的数据包交付给最终的UDP套接字。  

    + UDP的高效使用  
        虽然大部分网络编程都基于TCP实现，但也有一些是基于UDP实现的。网络传输特性导致信息丢失频发，可若要传递压缩文件(发送1万个数据包时，只要丢失1个就会产生问题)，则必须使用TCP，因为压缩文件只要丢失一部分就很难解压。但通过网络实时传输视频或音频时的情况有所不同。对于多媒体数据而言，丢失一部分也没有太大问题，这只会引起短暂的画面抖动，或出现细微的杂音。但因为需要提供实时服务，速度就成为非常重要的因素。因此流控制就显得有些多余，此时需要考虑使用UDP。但UDP并非每次都快于TCP，TCP比UDp慢的原因通常有以下两点。  
            - 收发数据前后进行的连接设置及清除过程   
            - 收发数据过程中为保证可靠性而添加的流控制  

2. 实现基于UDP的服务器端/客户端  
    + UDP中的服务器端和客户端没有连接  
        UDP服务器端/客户端不像TCP那样在连接状态下交换数据，因此与TCP不同，无需经过连接过程。也就是说，不必调用TCP连接过程中调用的listen函数和accept函数。UDP中只有创建套接字的过程和数据交换过程。  

    + UDP服务器端和客户端均只需一个套接字  
        TCP中，套接字之间应该是一对一的关系。若要向10个客户端提供服务，则除了守门的服务器套接字外，还需要10个服务器端套接字。但在UDP中，不管是服务器端还是客户端都只需要1个套接字。
    + 基于UDP的数据I/O函数  
        创建好TCP套接字后，传输数据时无需再添加地址信息。因为TCP套接字将保持与对方套接字的连接。换言之，TCP套接字知道目标地址信息。但UDP套接字不会保持连接状态(UDP套接字只有简单的邮筒功能),因此每次传输数据都要添加目标地址信息。这相当于寄信前在信件中填写地址。  

        传输数据时调用的UDP相关函数  
        ``` C
        #include <sys/socket.h>

        ssize_t sendto(int sock, void *buff, size_t nbytes, int flag, struct sockaddr *to, socklen_t addrlen);

        /* 成功时返回传输的字节数，失败时返回-1 */
        ```

        接受UDP数据的函数  
        ``` C
        #include <sys/socket.h>

        ssize_t recvfrom(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);

        /* 成功时返回接受的字节数，失败时返回-1 */
        ```

    + 基于UDP的回声服务器端/客户端  
        需要注意的是，UDP不同于TCP，不存在请求连接和受理过程，因此在某种意义上无法明确区分服务器端和客户端。只是因其提供服务而称为服务器端。  

        [uecho_server.c](../../c-example/socket/Chap6/uecho_server.c)  

        接下来是客户端部分，这部分代码与TCP客户端不同，不存在connect函数调用。  

        [uecho_client.c](../../c-example/socket/Chap6/uecho_client.c)  


    + UDP客户端套接字的地址分配  
        仔细观察UDP客户端会发现，它缺少把IP和端口分配给套接字的过程。TCP客户端调用connet函数自动完成此过程，而UDP中连能承担相同功能的函数调用语句都没有。  
        UDP程序中，调用sendto函数传输数据前应完成对套接字的地址分配工作，因此调用bind函数。当然，bind函数在TCP程序中出现过，但bind函数不区分TCP和UDP，也就是说，在UDP程序中同样可以调用。另外，如果调用sendto函数时发现尚未分配地址信息，则在首次调用sendto函数时给相应套接字分配IP和端口。而且此时分配的地址一直保留到程序结束为止，因此也可用来与其他UDP套接字进行数据交换。当然，IP用主机IP，端口号选尚未使用的任意端口号。  
        综上所述，调用sendto函数时自动分配IP和端口号，因此，UDP客户端中通常无需额外的地址分配过程。  
        
3. UDP的数据传输特性和调用connect函数  
    + 存在数据边界的UDP套接字  
        前面说过TCP数据传输中不存在边界，这表示"数据传输过程中调用I/O函数的次数不具有任何意义。"  
        相反，UDP是具有数据边界的协议，传输中调用I/O函数的次数非常重要。因此，输入函数的调用次数应和输出函数的调用次数完全一致，这样才能保证接收全部已发送数据。  

        [bound_host1.c](../../c-example/socket/Chap6/bound_host1.c)  
        [bound_host2.c](../../c-example/socket/Chap6/bound_host2.c)  

        bound_host2.c程序3次调用sendto函数以传输数据，bound_host1.c则调用3次recvfrom函数以接收数据。recvfrom函数调用间隔为5秒，因此，调用recvfrom函数前已调用了3次sendto函数。也就是说，此时数据已经传输到bound_host1.c。如果是TCP程序，这时只需调用1次输入函数即可读入数据。UDP则不同，在这种情况下也需要调用3次recvfrom函数。  

        UDP套接字传输的数据包又称数据报，实际上数据报也属于数据包的一种。只是与TCP包不同，其本身可以成为1个完整数据。这与UDP的数据传输特性有关，UDP中存在数据边界，1个数据包即可成为一个完整数据，因此称为数据报。  

    + 已连接(connected)UDP套接字与未连接(unconnected)UDP套接字接口  
        TCP套接字中需注册传输数据的目标IP和端口号，而UDP中则无需注册。因此，通过sendto函数传输数据的过程大致可分为以下3个阶段。  
            1. 向UDP套接字注册目标IP和端口号。  
            2. 传输数据。  
            3. 删除UDP套接字中注册的目标地址信息。  
        每次调用sendto函数时重复上述过程。每次都变更目标地址，因此可以重复利用同一UDP套接字向不同目标传输数据。这种未注册目标地址信息的套接字称为未连接套接字，反之，注册了目标地址的套接字称为连接connected套接字。显然，UDP套接字默认属于未连接套接字。但UDP套接字在下述情况下显得太不合理:  

        >ip为211.210.147.82的主机82号端口共准备了3个数据，调用3次sendto函数进行传输  
        
        此时需要重复3次上述三阶段。因此，要与同一主机进行长时间通信时，将UDP套接字变成已连接套接字会提高效率。上述三个阶段中，第一个和第三个阶段占整个通信过程近1/3的时间，缩短这部分时间将大大提高整理性能。   

    + 创建已连接UDP套接字  
        创建已连接UDP套接字的过程格外简单，只需针对UDP套接字调用connect函数。  

        ``` C
        sock = socket(PF_INET, SOCK_DGRAM, 0);
        memset(&adr, 0, sizeof(adr));
        adr.sin_family = AF_INET;
        adr.sin_addr.s_addr = ...
        adr.sin_port = ...
        connect(sock, (struct sockaddr *)&adr, sizeof(adr));
        ```
        上述代码看似与TCP套接字创建过程一致，但socket函数的第二个参数分明是SOCK_DGRAM。也就是说，创建的的确是UDP套接字。当然，针对UDP套接字调用connect函数并不意味着要与对方UDP套接字连接，这只是向UDP套接字注册目标IP和端口信息。  
        
### 7. 优雅的断开套接字连接
1. 基于TCP的半关闭  
    TCP中的断开连接过程比建立连接过程更重要，因为连接过程中一般不会出现大的变数，但断开过程有可能发生预想不到的情况，因此应准确掌控。只有掌握了下面要讲解的半关闭(Half-close)，才能明确断开过程。  
    + 单方面断开连接带来的问题  
        Linux的close函数和Windows的closesocket函数意味着完全断开连接。完全断开不仅指无法传输数据，而且也不能接收数据。因此，在某些情况下，通信一方调用close或closesocket函数断开连接就显得不太优雅。  
        为了解决这类问题，“只关闭一部分数据交换中使用的流”(Half-close)的方法应运而生。断开一部分连接是指，可以传输数据但无法接收，或可以接收数据但无法传输。顾名思义就是只关闭流的一半。   

    + 套接字和流(Stream)  
        两台主机通过套接字建立连接后进入可交换数据的状态，又称"流形成的状态"。也就是把建立套接字后可交换数据的状态看作一种流。  
        此处的流可以比作水流。水朝着一个方向流动，同样，在套接字的流中，数据也只能向一个方向移动。因此，为了进行双向通信，需要如下两个流:  

        >-------read() <====I/O流1==== write()  
        >HostA---------------------------------HostB  
        >-------write() ====I/O流2====> read()  

        一旦两台主机间建立了套接字连接，每个主机就会拥有单独的输入流和输出流。当然，其中一个主机的输入流与另一个主机的输出流相连，而输出流则与另一主机的输入流相连。另外，"优雅地断开连接方式"指断开其中1个流，而非同时断开两个流。Linux的close和Windows的closesocket函数将同时断开这两个流。  

    + 针对优雅断开的shutdown函数  
        接下来介绍用于半关闭的函数。下面这个shutdown函数就用来关闭其中1个流。  

        ``` C
        #include <sys/socket.h>

        //sock：需要断开的套接字文件描述符
        //howto：传递断开方式信息
        //成功时返回0，失败时返回-1
        int shutdown(int sock, int howto);
        ```

        调用上述函数时，第二个参数决定断开连接的方式，其可能值如下:  

        - SHUT_RD:断开输入流  
        - SHUT_WR:断开输出流  
        - SHUT_RDWR:同时断开I/O流  
        
        若向shutdown的第二个参数传递SHUT_RD，则断开输入流，套接字无法接收数据。即使输入缓冲收到数据也会抹去，而且无法调用输入相关函数。如果向shutdown函数的第二个参数传递SHUT_WR，则中断输出流，也就无法传输数据。但如果输出缓冲还留有未传输的数据，则将传递至目标主机。最后，若传入SHUT_RDWR，则同时中断I/O流。这相当于2次调用shutdown，其中一次以SHUT_RD为参数，另一次以SHUT_WR为参数。  

    + 为何需要半关闭  

    + 基于半关闭的文件传输程序  
        首先介绍服务器端  
        [file_server.c](../../c-example/socket/Chap7/file_server.c)  
        服务端程序  
        [file_client.c](../../c-example/socket/Chap7/file_client.c)  

### 8. 域名及网络地址
1. 域名系统  
    + 什么是域名  
    + DNS服务器  

2. IP地址和域名之间的转换  
    本节介绍通过程序向DNS服务器发出解析请求的方式。  
    + 利用域名获取IP地址  
        ``` c
        #include <netdb.h>

        struct hostent *gethostbyname(const char *hostname);
        /* 成功时返回hostent结构体地址，失败时返回NULL指针 */

        struct hostent
        {
            char *h_name;       //official name \0结尾
            char **h_aliases;   //alias list 字符串数组，最后一个为NULL
            int h_addrtype;     //host address type
            int h_length;       //address length
            char **h_addr_list; //address list ip地址数组，最后一个为NULL
        }
        ```

        [gethostbyname.c](../../c-example/socket/Chap8/gethostbyname.c)  

    + 利用IP获取域名地址  
        ``` c
        #include <netdb.h>

        struct hostent *gethostbyaddr(const char *addr, socklen_t len, int family);
        ```

        [gethostbyaddr.c](../../c-example/socket/Chap8/gethostbyaddr.c)  

### 9. 套接字的多种可选项
1. 套接字可选项和I/O缓冲大小  
    + 套接字多种可选项  
    
        |协议层|选项名|读取|设置|
        |:-:|:-:|:-:|:-:|
        |SOL_SOCKET|SO_SNDBUF|O|O|
        |--|SO_RCVBUF|O|O|
        |--|SO_REUSEADDR|O|O|
        |--|SO_KEEPALIVE|O|O|
        |--|SO_BROADCAST|O|O|
        |--|SO_DONTROUTE|O|O|
        |--|SO_OOBINLINE|O|O|
        |--|SO_ERROR|O|X|
        |--|SO_TYPE|O|X|
        |IPPROTO_IP|IP_TOS|O|O|
        |--|IP_TTL|O|O|
        |--|IP_MULTICAST_TTL|O|O|
        |--|IP_MULTICAST_LOOP|O|O|
        |--|IP_MULTICAST_IF|O|O|
        |IPPROTO_TCP|TCP_KEEPALIVE|O|O|
        |--|TCP_NODELAY|O|O|
        |--|TCP_MAXSEG|O|O|

        IPPROTO_IP层可选项是IP协议相关事项，IPPROTO_TCP层可选项是TCP协议相关的事项，SOL_SOCKET层套接字相关的通用可选项。  

    + getsockopt&setsockopt  

        ``` c
        #include <sys/socket.h>

        int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);
        /* 成功时返回0,失败时返回-1 */

        int setsockopt(int sock, int level, int optname, void *optval, socklen_t optlen);
        ```

        [sock_type.c](../../c-example/socket/Chap8/sock_type.c)  

    + SO_SNDBUF&SO_RCVBUF  
        SO_RCVBUF是输入缓冲大小相关可选项，SO_SNDBUF是输出缓冲大小相关可选项。  

        [get_buf.c](../../c-example/socket/Chap8/get_buf.c)  
        [set_buf.c](../../c-example/socket/Chap8/set_buf.c)  

2. SO_REUSEADDR  
    + 发生地址分配错误(Binding Error)  

        [reuseadr_eserver.c](../../c-example/socket/Chap8/reuseadr_eserver.c)  

        服务端控制台输入CTRL+C关闭服务器后，服务器端重新运行时将产生问题。用同一端口号重新运行服务器端，将输出"bind() error"，并且无法再次运行

    + Time-wait状态  
        假设主机A和主机B建立连接后，当主机A在发送ACK响应主机B的FIN后立刻消除套接字。然而这条ACK消息再传递途中丢失，未能传给主机B。这时主机B会认为之前自己发送的FIN消息未能抵达主机A，继而试图重传。但此时主机A已是完全终止状态，主机B永远无法收到主机A最后传来的ACK消息。而如果，主机A的套接字处在Time-wait状态，则会向主机B重传最后的ACK消息，主机B也可以正常终止。因此，先传输FIN消息的主机应经过Time-wait过程。  

    + 地址再分配  
        如果主机B重传FIN后，主机A收到FIN报文后会重启Time-wait的定时器，这样在网络状态不理想时，Time-wait状态将持续。  
        解决方案就是在套接字的可选项中更改SO_REUSEADDR的状态。时当调整该参数，可将Time-wait状态下的套接字端口重新分配给新的套接字。SO_REUSEADDR的默认值为0,因此需要改成1。  

3. TCP_NODELAAY  
    + Nagle算法  
        只有收到前一数据的ACK消息时，Nagle算法才发送下一数据  

    + 禁用Nagle算法  
        设置TCP_NODELAY为1  

### 10. 多进程服务器端
1. 进程概念及应用  
    + 两种类型的服务器端  
        * 第一个连接请求的受理时间为0秒，第50个连接请求的受理时间为50秒，第100个连接请求的受理时间为100秒!但只要受理，服务只需1秒钟。  
        * 所有连接请求的受理时间不超过1秒，但平均服务时间为2-3秒。  
        
    + 并发服务器端的实现方法  
        即使有可能延长服务时间，也有必要改进服务器端，使其同时向所有发起请求的客户端提供服务，以提高平均满意度。而且，网络程序中数据通信时间比CPU运算时间占比更大，因此，向多个客户端提供服务是一种有效利用CPU的方式。下面是具有代表性的并发服务器端实现模型和方法：  
        * 多进程服务器:通过创建多个进程提供服务。  
        * 多路复用服务器：通过捆绑并统一管理I/O对象提供服务。  
        * 多线程服务器：通过生成与客户端等量的线程提供服务。  

    + 理解进程  
        进程:占用内存空间的正在运行的程序  
        从操作系统的角度看，进程是程序流的基本单位，若创建多个进程，则操作系统将同时运行。有时一个程序运行过程中也会产生多个进程。接下来要创建的多进程服务器就是其中的代表。编写服务器端前，先了解一下通过程序创建进程的方法。  

    + 进程ID  
        所有进程都会从操作系统分配到ID。此ID称为"进程ID"，其值为大于2的整数。1要分配给操作系统启动后的首个进程，因此用户进程无法得到ID值1。  

    + 通过调用fork函数创建进程  
        创建进程的方法很多，此处只介绍用于创建多进程服务器端的fork函数。  

        ``` C
        #include <unistd.h>

        pid_t fork(void);
        /* 成功时返回进程ID，失败时返回-1 */
        ```

        fork函数将创建调用的进程副本。也就是说，并非根据完全不同的程序创建进程，而是复制正在运行的，调用fork函数的进程。另外，两个进程都将执行fork函数调用后的语句。但因为通过同一个进程、复制相同的内存空间，之后的程序流要根据fork函数的返回值加以区分。即利用fork函数的如下特点区分程序执行流程。  

        * 父进程:fork函数返回子进程ID。  
        * 子进程:fork函数返回0  
        
        [fork.c](../../c-example/Chap10/fork.c)  
        
2. 进程和僵尸进程  
3. 信号处理  
4. 基于多任务的并发服务器  
5. 分割TCP的I/O程序  
### 11. 进程间通信
1. 进程间通信的基本概念  
2. 运用进程间通信  
### 12. I/O复用
1. 基于I/O复用的服务器端  
2. 理解select函数并实现服务器端  
### 13. 多种I/O函数
1. send&recv函数  
2. readv&writev函数  
### 14. 多播与广播
1. 多播  
2. 广播  
### 15. 套接字和标准I/O
1. 标准I/O函数的优点  
2. 使用标准I/O函数  
3. 基于套接字的标准I/O函数使用  
### 16. 关于I/O流分离的其他内容
1. 分离I/O流  
2. 文件描述符的复制和半关闭  
### 17. 优于select的epoll
1. epoll理解及应用  
2. 条件触发和边缘触发  
### 18. 多线程服务器端的实现
1. 理解线程的概念  
2. 线程创建及运行  
3. 线程存在的问题和临界区  
4. 线程同步   
5. 线程的销毁和多线程并发服务器端的实现  
### 24. 制作HTTP服务器端
1. HTTP概要  
2. 实现简单的Web服务器端  