---
title: "Gdb Note"
date: 2021-06-01T13:54:17+08:00
Description: "主要为一些常用的命令总结"
Tags: ["调试工具"]
Categories: ["Linux"]
DisableComments: false
---

gdb简单介绍即常用命令总结  

<!--more-->
### 1. GDB简介  
GDB是基于ptrace这个系统调用来实现的。其原理是利用ptrace系统调用，在被调程序和GDB之间建立追踪关系。然后所有发送给被调试程序的信号都会被GDB截获，GDB根据截获的信号，查看被调试程序相应的内存地址，并控制被调试的程序继续运行。  

#### 建立调试关系  
包括两种模式，使用GDB启动程序，以及attach到运行进程。  

1. fork：利用fork+execve执行被调试的程序，子进程在执行execve之前调用ptrace(PTRACE_TRACEME)，建立了与父进程(debugger)的跟踪关系。  
2. attach：debugger可以调用ptrace(PTRACE_ATTACH,pid,...)，建立自己与进程号为pid的进程间的跟踪关系。即利用PTRACE_ATTACH,使自己变成被调试程序的父进程。用attach建立起来的跟踪关系，可以调用ptrace(PTRACE_DETACH,pid,...)来解除。  

#### 断点原理  

1. 断点的实现原理:插入断点指令，运行到断点处并产生SIGTRAP信号，GDB捕获信号并进行断点命中判定，若命中则转入等待用户输入进行下一步处理状态，否则继续运行。  
2. 断点的设置原理:先将断点处原本的指令保存，然后向该位置写入int 3。执行到该位置后发生软中断，内核会发出SIGTRAP信号给子进程并被转发给父进程。然后用保存的指令替换int 3。  
3. 断点命中判断:GDB会将所有断点位置存放在一个链表中，将被调试程序当前停止的位置和链表中的断点位置进行比较，看是否为断点产生的信号。  
4. 条件断点的判定:在恢复断点处的指令后，除了命中判断外，还会加一步条件判断，若为真则触发断点。  

#### 单步跟踪原理  
ptrace本身支持单步调试功能，调用ptrace(PTRACE_SINGLESTEP,pid,...)即可实现。  

#### GDB接口  
GDB有两类接口MI(Machine Interface)和CLI(Command line Interface)接口。  

#### GDB调试符号  
gcc在编译时，不会将调试符号插入到生成的二进制代码中，使用-g的选项才会在编译时生成调试符号信息。  
gcc在产生调试符号时，同样采用了分级的思路可以通过-g1/-g2(默认级别)/-g3/-g4来指定编译时加入的调试信息的多少。  

#### GDB启动  
启动GDB的方法有以下几种:  

1. gdb <program>  
2. gdb <program> core  
3. gdb <program> <PID>或gdb -p <PID>  

#### GDB退出  

1. quit或者Ctrl+d退出。  
2. (Ctrl+c)中断并不是从GDB退出，而是终止正在执行GDB然后返回到GDB命令行。  
3. 使用gdb attach的可以使用detach命令释放。  

#### Shell命令  
在调试期间，要偶尔执行shell命令，不用离开或暂停GDB，可以直接使用shell命令。shell command-string或者!command-string会调用标准shell以执行命令字符串。  

#### 日志记录  
GDB有日志记录功能，可以将GDB命令的输出保存到文件中。相关命令如下：  

>set logging on   启动日志记录
>set logging off  关闭日志记录
>set logging file file 更改日志文件的名称，默认为gdb.txt  

### 2. 常用基本命令  

1. 断点类命令  
    help breakpoints  
2. 数据类命令  
    help data  
3. 文件类命令  
    help files  
4. 运行类命令  
    help running  
5. 栈类命令  
    help stack  
6. 信息类命令  
    help status  

### 3. 调试运行程序  
调试运行的进程是动态的，可以单步执行，观察变量信息等，还可以捕捉到一些异常信息。分为GDB启动程序和GDB附加运行程序，GDB启动程序常常用于定位分析程序启动时候的一些异常，GDB附加运行程序主要用于定位运行中的异常。  
#### GDB启动程序  

>gdb <program>  
>run [arg1] [arg2] [arg3] [arg4]  

1. 断点
    commands可以在断点处设置命令行来自动打印信息。  
    set pagination off可以关闭分页功能。  
    save breakpoints [filename]可以保存断点信息。  
    source [filename]可以增加filename中记录的断点。  
2. 查看/打印信息  
    info locals查看局部变量当前值。  
    info registers查看当前寄存器的值。  
    u [linenum]跳转到指定的行号。  
    x x /[n|f|u] addr 表示以addr为起始地址，返回n个单元的值，每个但愿对应u个字节，输出格式是f。  
    ptype和whatis查看变量的类型，如果是结构体，ptype会显式结构体成员，whatis显式变量声明时候的类型。  
3. 检测点(watch)  
    GDB还可以监测某个地址数据的变化以及访问情况。watch是监测到内存值发生变化的时候中断，rwatch是监测到内存被读的时候中断，awatch是监测被读或被写的时候中断，值发生改动的时候会打印出变化前后内容，其中rwatch和awatch是基于CPU的硬件断点。  
4. 检查点(checkpoint)  
    有些时候一些bug，可能很难复现，当好不容易复现一次，且刚刚进入程序的入口时，我们需要珍惜这个来之不易的机会。有时候，使用step命令一步一步的太慢，而一不小心使用了next，结果恰好该函数调用返回出错。那么对于这次的机会，只知道问题出现在该函数，但是哪里出错还不清楚。于是还需要再一次的复现bug，这样会浪费大量时间，因此checkpoint就发挥作用了。  
    checkpoint 标记当前检查点。  
    info checkpoints 查看当前检查点。  
    restart 1 恢复到1号检查点状态。  
    checkpoint不支持多线程，程序被终止那么快照会消失。  
5. 信号处理  
    GDB调试程序，有时候会收到很多信号导致程序不停的中断，这个时候可以通过handle signalno action命令来告诉GDB需要处理那一种信号。  
    signalno：可以以SIG开头也可以不以SIG开头，还可以定义一个要处理信号的范围(如SIGIO-SIGKILL),还可以使用关健字all来标明要处理所有的信号。  
    action:可以由下列的一个或多个关键字组成。nostop、stop、print、noprint、pass/noignore、nopass/ignore  

    >handle SIGCONT nostop noprint

#### GDB附加运行中程序
可以gdb -p PID这样附加进程，也可以通过attach PID附加进程。PID可以通过top或者ps命令查找到进程ID。  

1. 多线程  
    多线程可以通过info threads查看线程信息，行首为GDB分配的线程ID号，对线程进行切换时，使用该ID号码。另外，行首的\*号标识了当前活动的线程。使用thread THREADNUMBER进行切换，THREADNUMBER为上文的ID号。  
    还可以通过thread apply all bt命令查看所有线程堆栈信息，从而找到异常的线程。可以通过bt命令查看堆栈，bt n显式前n条，bt -n则显式后面的n条堆栈，bt full查看堆栈的同时显式局部变量信息。  
    多线程中有些函数会被很多线程调用，如果设置的断点没有指定线程则每次进入函数的时候都会断住，可能会有很多无用的断点，可以使用break fun|file thread THREADNUMBER来指定线程。  

    >b malloc thread 10  

    多线程断点断住的时候，默认其他所有的线程也会停止运行，即all-stop mode，如果希望某一个线程停止时，其他线程会继续运行，则可以通过命令set non-stop on设置，命令show non-stop查看当前的开关状态。  
    all-stop mode模式下，当用单步调试命令step或next，所有的线程开始执行。由于执行线程调度的是操作系统不是GDB，单步调试命令不能让所有的线程都单步。当前线程执行了一步，其他线程可能执行了N步。  
    可以通过set scheduler-locking命令设置调度锁定模式，在一些系统中，GDB可以通过锁定操作系统线程调度，只允许一个线程运行。如果是on，只有当前被调试程序会执行。如果是off，所有线程都会运行。如果是step，则在单步的时候只有当前线程执行，当执行continue/util/finish时，其他线程会恢复运行。执行show scheduler-locking显式当前线程调度锁定状态。  
2. 多进程  
    GDB调试多进程的时候，默认设置下只会调试主进程，通过设置follow-fork-mode就可调试多个进程，set follow-fork-mode child然后进入调试子进程，这种方式只能同时调试一个进程。show follow-fork-mode命令查看当前调试的fork的模式。  
    还可以通过设置detach-on-fork决定GDB是同时调试父子程序，还是在fork了子进程之后，将子进程分离出去。设置为on的话，只调试父进程或子进程其中一个，需要根据follow-fork-mode决定这是默认模式;设置成off的话，父子进程都在GDB的控制之下，其中一个进程正常调试，需要根据follow-fork-mode决定，另一个进程会被设置为暂停状态。show detach-on-fork命令查看detach-on-fork的模式。  
    通过info inferiors命令查看GDB调试的所有子进程，GDB会为他们分配ID。其中带有\*的进程是正在调试的子进程。inferior num可以切换到ID为num的子进程进行调试。detach inferior infno命令会detach掉编号是infno的子进程。这个子进程还存在，可以再次用run命令执行它。  
    kill inferior infno：kill掉infno号子进程。这个进程仍然存在，可以再次用run等命令执行它。  
    remove-inferior infno:删除一个infno号的子进程。需要先kill或detach这个子进程后才能删除。  
    schedule-multiple：设置子进程的调度，设为off只有当前inferior会执行，设为on则执行状态的inferior都会执行。类似于多线程调试里的set scheduler-locking选项。如果scheduler-locking是on，即使schedule-multiple设置为on，也只有当前进程的当前线程会执行。
    set follow-exec-mode mode：用来跟踪这个exec装载的程序，当发生exec的时候，如果这个选项mode是new，则新建一个inferior给执行起来的子进程，而父进程的inferior仍然保留，当前保留的inferior的程序状态是没有执行。当发生exec的时候，如果这个选项mode是same(默认值),因为父进程已经退出，所以自动在执行exec的inferior上控制子进程。  
    maint info program-spaces：显式当前GDB一共管理了多少地址空间。  

### 4. 分析core文件  
#### core文件相关配置  
常说程序dump掉了，需要定位解决，大部分的情况是指对应程序由于各种异常或者bug导致在运行过程中异常退出或中止，并且在满足一定条件下会产生一个core文件。  
通常情况下，core文件会包含程序运行时的内存、寄存器状态、堆栈指针、内存管理信息还有各种函数调用堆栈信息等，可以理解为是程序当前工作状态存储生成的镜像文件。  

+ core文件的生成开关和大小限制  
    ulimit -c命令可查看core文件的生成开关。若结果为0,则表示关闭了此功能，不会生成core文件。  
    ulimit -c filesize命令，可以限制core文件的大小，其中filesize单位为kbyte。  
    ulimit -c unlimited表示设置core文件的大小不受限制，这样能产生一个完整的core文件。  
+ core文件的生成路径  
    默认下与可执行文件运行时的路径相同。若系统生成的core文件不带其它任何扩展名称，则全部命名为core。新的core文件生成将覆盖原来的core文件。  
    cat /proc/sys/kernel/core_pattern可以查看core文件的默认位置，默认文件位置通过/usr/libexec/abrt-hook-ccpp脚本实现。  
+ 控制core文件的文件名中是否添加pid作为扩展  
    文件/proc/sys/kernel/core_uses_pid可以控制core文件的文件名中是否添加pid作为扩展。文件内容为1,表示添加pid作为扩展名，生成的core文件格式为core.xxxx;为0则表示生成的core文件统一命名为core。  
+ 修改core dump文件保存路径和文件名格式
    临时修改：修改/proc/sys/kernel/core_pattern文件。  

    >echo "./core-%e-%p-%t" > /proc/sys/kernel/core_pattern  

    永久修改：使用sysctl -w name = value命令。  

    >/sbin/sysctl -w kernel.core_pattern=./core-%e-%p-%t  

#### 手动生成core文件  
先用GDB附加到相应的进程中，然后使用gcore core_name命令生成coredump文件core_name，如果没有指定路径则coredump文件存放当前目录。

>(gdb)gcore core_978

或者在操作系统上是用gcore命令生成coredump文件，原理也是使用GDB附加到进程生成core文件。

>/usr/bin/gcore -o core_978 978

还可以通过kill -s 11 pid异常信号方法生成coredump文件，原理是向进程发送一个段错误信号;一般情况下程序发生段错误会生成coredump文件，同时程序也会退出执行。  

>kill -s 11 6200

#### 分析core文件  

+ 确定core文件和GDB  
    coredump文件本身主要的格式也是ELF格式，因此，我们可以通过readelf命令进行判断。  
    找到core文件，为什么还需要确定GDB呢，因为存在交叉编译，所以必须使用相应的交叉调试。即运行的CPU框架可能与实际分析core文件的CPU框架不一致。  
    比如core文件是在PowerPC平台，因此需要PowerPC平台的GDB，一般情况下交叉分析core文件都是在编译服务器上进行的，编译服务器上能够交叉编译通常也安装了相应的GDB版本。  

+ GDB分析core文件的步骤  
    使用GDB加载符号文件和映像文件，gdb 进程文件 core文件。  
    GDB加载core文件的时候一般会提示程序是由于什么原因产生coredump，例如SIGSEGV。  
    指定运行库目录。对于运行时库文件与当前系统库文件不一样的情况需要执行这个步骤，才能正确解析调用关系。  

    >set solib-absolute-prefix /lib/

    加载符号表(如果可执行文件中没有符号表或者使用strip剪裁了)，需要加载带有符号表的可执行文件以及库文件，file可执行文件;通常GDB会在当前目录或者debug目录(show debug-file-directory查看),搜索依赖的符号表文件。  
    加载源码，如果需要根据源码进行调试，需要加载源码并查看源码，dir 路径。  
    使用bt查看出错的函数调用栈，找到出现异常的函数。  
    查看反汇编代码，通过disassemble /m add命令查看反汇编，并找到当前异常的指令位置，并找到异常的c代码。  

+ coredump产生可能原因  
    - 内存访问越界。  
    - 多线程程序使用了线程不安全的函数，因此推荐使用安全的可重入函数。  
    - 多线程读写的数据未加锁保护。  
    - 非法指针。  
    - 堆栈溢出，不要使用大的局部变量，这样容易造成堆栈溢出，破坏系统的栈和堆结构。  
    - 断言，对应的信号为SIGABRT，程序主动调用abort函数时产生此信号。

### 5. GDB脚本  
在线调试时GDB脚本很有用处，使业务流程不被中断的情况下产生需要的调试信息，在不需要人工干预的情况下通过set logging file FILENAME和set logging on来实现将调试信息输出到文件。  
常用GDB调试脚本有两种，一种是直接写GDB命令，一种是python脚本。  
GDB命令方式更直接且方便，但是支持的逻辑有限只有if、while、loop等几种，并且缺乏对文本的解析。  
GDB家在脚本的方式有：  

+ autoload方式  
    需要把教本放置到/usr/share/gdb/auto-load/usr/lib目录下。
+ gdb -x script方式  
+ gdb命令source script方式  

#### 命令脚本  
首先对于命令脚本的命名，其实GDB没有什么特殊要求，只要文件名不是GDB支持的其他脚本语言的文件名就可以了。  
#### 基本语法  

1. \# 为脚本注释命令  
2. 赋值语句：set，变量以$开始，以便区分GDB还是调试程序变量。变量可为全局或局部，视声明位置。变量不需要声明，变量类型自动继承于赋值类型。  

>set $i = 1 //设置i的值为1

3. 函数声明语句：define...end  

>define func
>   statement
>end

其中statement可以是任意GDB命令。无形参声明，此外自定义命令还支持最多10个输入参数($arg0,$arg1,$arg2...$arg9),并且还用$argc来标明一共传入了多少参数。  

4. 说明语句：document...end，为函数书写帮助说明个是如下：  

>document nbase_av13_tree
>   nbase_av13_tree
>   Examples:
>   nbase_av13_tree tree CB_TYPE var1 var2...
>end

5. 显式语句可能用print、printf，也可以用echo。其中printf可以和c语言一样带格式化打印，如果是p，默认调用的是print命令。  

>printf"%x",$subpool
>printf"%d"(int)$subpool->subpool_size

6. 条件命令：if...else...end。
7. 循环命令：while...end。GDB同样提供了loop_break和loop_continue命令分别对应其它语言中的break和continue。  
8. 支持宏定义macro define，如下所示，这样每次执行nbs_thread_group也就是取$glb->nbs_thread_group的值，注意使用的时候只能是nbs_thread_group,不能$nbs_thread_group。  

>macro define nbs_thread_group  $glb->nbs_thread_group

9. if和while后面的条件判断跟c语言一样，多个条件的时候用&&、||等。注意不同类型的变量，需要强转之后才能比较，例如两个不同类型指针比较，需要将指针强转为int。 
10. 常规的GDB调试命令语句。  
11. 支持shell命令，可以实现dump，search等复杂功能。  
12. 每一行是一句话，GDB脚本每行是可以在GDB上单独执行调试的。  

***注意GDB中使用的变量要加上'$'符号，否则会认为是被调试的程序中的变量。***  
***GDB脚本不支持c语言的i++类似写法***  

#### 示例脚本  
示例脚本(search_byte.gdb)定义了search_byte命令，功能是在一段指定内存查找一个值，需要输入内存的起始地址，结束地址和要找的值。  

```gdb
define search_byte
    if $argc!=3
        help search_byte
    else
        set $begin_addr=$arg0
        set $end_addr=$arg1

        while $begin_addr<=$end_addr
            if *((unsigned int*)$begin_addr)==$arg2
                printf "Find it!The address is 0x%x\n",$begin_addr
                loop_break
            else
                set $begin_addr=$begin_addr+1
            end
        end

        if $begin_addr>$end_addr
            printf"Can't find it!\n"
        end
    end
end

document search_byte
    search a specified byte value(0~255)during a memory
    usage:search_byte begin_addr end_addr byte
end
```

### 远程调试  
gdbserver是gdb的一个服务端，那gdb必然就是想对应的客户端了。这两者之间是TCP连接，gdb发送命令给gdbserver，gdbserver收到命令后会控制进程进行相对应的操作，然后把操作结果反馈给GDB，实现对目标机上的系统内核和上层应用的监控和调试功能。  

1. 启动gdbserver  
    gdbserver支持单进程和多进程模式。  
    + 单进程模式  
        单进程模式就是gdbserver已经附加到某个进程上，通过gdbserver --attach：监听端口号 PID，其中PID是当前正在运行的进程，例如gdbserver --attach ：5555 13571。  
    + 多进程模式
        用target remote连接gdbserver，gdbserver只会调试指定的程序，如果程序退出或者从程序脱离的话，GDB会断开连接，然后gdbserver就会退出。  
        用target extended-remote来连接，gdbserver会进入多进程模式。如果被调试的程序退出或者从程序脱离的话，gdb会保持和gdbserver的连接，即使没有程序运行也会保持连接。可以通过run或attach命令来运行或者附加新的进程。run命令使用set remote exec-file来指定运行程序。gdbserver不会自动退出，可以使用monitor exit命令结束它。  
        启动gdbserver使用--multi选项就不需要带初始命令或者需要附加的进程号。然后就可以使用target extended-remote连接并启动需要调试的程序。如gdbserver --multi :1234 &后台启动。  

2. 连接gdbserver  
    连接到远程目标的基本过程如下：  
    + 在主机系统上运行gdb。  
    + 确认有必须的符号文件。在连接之前，用file命令为应用程序加载符号。用设置sysroot来定位目标库。本地主机和远端设备的可执行文件必须完全一致。  
    + 连接目标。对于TCP连接，必须先启动gdbserver，再使用target remote连接。  
    在目标程序远程调试完时，可以使用detach命令将其从GDB中释放，程序分离之后通常会继续执行。detach命令之后，GDB还可以继续连接其他的目标程序。disconnect命令和detach类似，除了目标程序通常不继续执行外。关闭与目标的连接，然后目标程序通常不会恢复。它将等待GDB连接并继续调试。在断开连接命令之后GDB再次自由连接到另一个目标程序。  

3. 串口调试  

+ gdbserver hello /dev/ttyS0  
+ 在pc机上运行命令:xxx-linux-gdb hello  
+ 在xxx-linux-gdb里敲入以下命令:  

>set remote device /dev/ttyS0 #设置串口1
>set remote baud 9600 #设置串口波特率
>set debug remote 1
>target remote /dev/ttyS0

4. 示例
远端启动gdbserver服务。
>gdbserver --multi :1234 &

本地启动gdb并连接到远端
>gdb
>target extended-remote 10.90.10.42:1234
>attach 13571