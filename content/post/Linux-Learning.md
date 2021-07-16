---
title: "Linux Learning"
date: 2020-09-08T12:24:17+08:00
Description: "记录Linux的相关学习笔记"
Tags: ["Linux"]
Categories: ["操作系统"]
DisableComments: false
---
主要是Linux学习笔记
<!-- more -->
### 1. Linux内存管理  

1. Linux进程空间概述  
Linux操作系统采用虚拟内存管理技术，使得每个进程都有各自互不干涉的进程地址空间。该空间是一段线性虚拟空间，用户所看到和操作的都是该虚拟地址，无法操作实际的物理内存地址。  

+ Linux进程空间(x86)  
    程序访问的地址都是虚拟地址，对于32位操作系统来讲，系统访问的地址空间为4G，linux会将4G地址空间分为两部分，其中从0x00000000到0xbfffffff的线性地址为用户空间，0xc0000000到0xffffffff为内核空间，进程在用户态只能访问0～3G，只有进入内核态才能访问3G～4G。  

    * 用户空间  
        在Linux中，从0-3GB的虚拟地址是用户空间，通过每个进程自己的页目录、页表，用户进程可以直接访问。
    * 内核空间  
        从3GB到4GB的虚拟地址为内核态空间，存放供内核访问的代码和数据，用户态进程不能访问，也就是说对于用户代码来说是不可见的，只有内核态进程才能寻址。所有进程从3GB到4GB的虚拟空间都是一样的，linux以此方式让内核态进程共享代码段和数据段(页表就存放在内核虚拟空间)。  

+ Linux进程空间(x86_64)  
    在x86_64架构下，地址长度为64位，但是虚拟地址只使用了48位，所以48位地址长度也就对应了256TB的地址空间。应用进程地址空间为0x000000000000~0x00007FFFFFFFFFFF,内核地址空间为0xFFFF800000000000~0xFFFFFFFFFFFFFFFF。  

2. Linux内存管理  

+ 虚拟内存管理  
    进程内存管理的对象是进程线性地址空间上的内存镜像，这些内存镜像其实就是进程使用的虚拟内存区域(memory region)。  
    Linux内核在创建进程的时候，会为每个进程分配一个进程描述符(task_struct)结构，进程描述符记录了该进程创建的内存空间信息(内存描述符mm_struct表示),由于线性地址可能不连续，一个进程会使用多个不连续的地址空间，所以用线性区(vm_area_struct)表示进程使用的一段连续地址，不同的线性区通过链表连接。  
    每一个线性区都是属性相同的连续地址段，不同属性的线性区通过vm_next链表链接，操作系统在给应用进程分配虚拟地址空间的时候，会把属性相同的临近地址进行合并。操作系统把应用进程内存空间所有线性区动态的存放到/proc/pid/maps文件中，通过命令pmap -x pid(cat /proc/pid/maps)可以查看该进程的能够访问的线性区以及线性区属性。  
+ 物理内存管理  
    Linux把物理内存划分为3个层次来管理:存储节点(Node)、管理区(Zone)和页面(Page)，并用3个相应的数据结构来描述。  
    - 存储节点(Node)  
        CPU访问物理内存中任何一个地址所需要的时间都相同，称为"一致性存储结构(Uniform MemoryArchitecture,UMA)"，而在一些新系统中多CPU结构的系统中，一致性却成了问题，所以有了"非一致存储结构(Non-UniformMemoryArchitecture,NUMA)"。为了对NUMA进行描述，引入一个新的概念"存储节点"，把访问时间相同的存储空间就叫做一个"存储节点"。  
        一般来说，连续的物理页面应该分配在相同的存储节点上。存储节点的数据结构为pglist_data，若干存储节点的pglist_data数据结构可以通过node_next形成一个单链表队列。每个结构中的node_mem_map指向具体节点的page结构数组，而数组node_zone[]就是该节点的3个页面管理区。   
    - 管理区(Zone)  
        Linux对物理内存的管理主要是为了解决从内核虚拟地址到物理地址的映射。IA32架构中内核虚拟地址空间只有1GB大小(从3G到4G)，因此可以直接将1GB大小的物理内存映射到内核地址空间，但超出1GB大小的物理内存就不能映射到内核空间。为了对物理页面进行有效的管理，Linux把物理页面划分为3个区:  
        1. 专供DMA使用的ZONE_DMA区(小于16MB)。  
        2. 常规的ZONE_NORMAL区(大于16MB小于896MB)。  
        3. 内核不能直接映射的区ZONE_HIGME区。  
        每个区都用struct zone_struct结构来表示。  
    - 页面(Page)  
        Linux物理内存管理以页为单位，它将整个内存划分成无数4k大小页，形成一个分页机制。内存中每个物理页面都有一个struct page结构，该结构包含了对物理页面进行管理的所有信息保存在一个巨大的page数组mem_map中。  
+ 缺页中断  
    用户进程所能直接操作的地址都为虚拟地址。当进程申请内存时，从内核获得的仅仅是虚拟的内存区域，而不是实际的物理地址，并没有获得物理内存，获得的仅仅是对一个新的线性地址区间的使用权。实际的物理内存只有当进程真的去访问新获得的虚拟地址时，才会由"请求页机制"产生"缺页"中断，从而进入分配实际页面的例程。  
    缺页中断是虚拟内存机制赖以存在的基本保证，它会告诉内核去真正为进程分配物理页，并建立对应的页表，这之后虚拟地址才实实在在地映射到了系统的物理内存上。  
    这种请求页机制把页面的分配推迟到不能再推迟为止，并不急于把所有的事情都一次做完。之所以能这么做是利用了内存访问的"局部性原理",请求页带来的好处是节约了空闲空间，提高了系统吞吐率。  
+ 内存映射  
    虽然应用程序操作的对象是映射到物理内存之上的虚拟内存，但是处理器直接操作的确是物理内存。所以当应用程序访问一个虚拟地址时，首先必须将虚拟地址转换成物理地址，然后处理器才能解析地址访问请求。地址的转换工作需要通过查询页表才能完成，概括地讲，地址转换需要将虚拟地址分段，使每段虚地址都作为一个索引指向页表，而页表项则指向下一级别的页表或者指向最终的物理页面。  
    每个进程都有自己的页表，进程描述符的pgd域指向的就是进程的页全局目录。具体流程如下：  

    1. 从cr3中取出进程的页目录地址(操作系统负责在调度进程的时候，把这个地址装入对应寄存器)。  
    2. 根据线性地址前十位，在数组中，找到对应的索引项，因为引入了二级管理模式，页目录中的项，不再是页的地址，而是一个页表的地址(又引入了一个数组)，页的地址被放到页表中去了。  
    3. 根据线性地址的中间十位，在页表中找到页的起始地址。  
    4. 将页的起始地址与线性地址中最后12位相加，得到最终的物理地址。  

    内存映射的相关说明如下:  

    1. 分页单元中，页目录是唯一的，它的地址放在CPU的r3寄存器中，是进行地址转换的开始点。  
    2. 每一个活动的进程，因为都有其独立的对应的虚拟内存(页目录也是唯一的)，那么它也对应了一个独立的页目录地址。运行一个进程，需要将它的页目录地址放到cr3寄存器中，并将其他进程的页目录地址保存下来。  
    3. 每一个32位的线性地址被划分为三部分，页目录索引(10位):页表索引(10位):页偏移(12位)。  

3. Linux堆内存  

+ 操作系统堆内存  
    操作系统把一大块内存作为堆内存供用户动态申请动态释放，而操作系统管理堆内存比较简单，主要由brk()系统调用来扩大堆内存，移动_edata指针来完成内存的申请。  

    * brk是将数据段的最高地址指针_edata往高地址推，入参为绝对地址，属于系统调用。  
    * sbrk是将数据段的最高地址指针_edata往高地址推，入参是指针相对偏移量，属于glibc函数调用。  
    brk分配内存是将_edata指针往高位地址推移一定的size，释放的时候是将_edata往低位地址推移一定的size。brk分配内存跟栈的管理类似，如果高地址的内存没有释放，低地址的内存无法归还给操作系统，因为只有一个_edata指针指向堆顶。以下代码可以演示brk的申请过程。  

    ```C
    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>

    int main(void)
    {
        int *A;
        int *D;
        int *p;
        int *array[400];
        int i = 0;
        void *start;
        A = sbrk(1024 * 10);
        start = sbrk(0);
        printf("after A sbrk(10K),_edata is %p\n",start);
        for(i = 0;i < 400;i++)
        {
            array[i] = malloc(1024 * 100);
        }
        start = sbrk(0); //得到当前_edata地址
        printf("after B/C malloc,_edata is %p\n",start);
        brk(start + 1024*20);//将_edata地址向高地址偏移10k，也就是申请了10k的堆内存
        D = sbrk(0)-1024*20;
        start = sbrk(0);
        printf("after D brk(20K),_edata is %p\n",start);
        for(i = 0;i < 400; i++)//释放malloc申请的内存
        {
            free(array[i]);
        }
        start = sbrk(0);
        printf("after B/C free,_edata is %p\n",start);//尽管中间B/C被释放，但是又于D未被释放故未将B/C归还给操作系统
        brk(D);//释放D申请的内存，把_edata往低地质推
        p = malloc(1024 * 100);//这里触发malloc再次释放内存到操作系统
        free(p);
        start = sbrk(0);
        printf("after D free,_edata is %p\n",start);
        return 0;
    }
    ```

    运行结果如下:
    >after A sbrk(10K),_edata is 0x55f1c478c800
    >after B/C malloc,_edata is 0x55f1c6ebf000
    >after D brk(20K),_edata is 0x55f1c6ec4000
    >after B/C free,_edata is 0x55f1c6ec4000
    >after D free,_edata is 0x55f1c47ad000

    B/C释放之后，_edata并没有移动，所以B/C占用的内存并没有释放给操作系统，只有等到顶部的D被释放后才能归还给操作系统。  
    
+ Glibc堆内存管理  
    brk属于系统调用，若每次申请内存，都调用brk，那么每次都会产生系统调用，影响性能;其次，这样申请的内存容易产生碎片，因为堆是从低地址到高地址，如果高地址的内存没有被释放，低地址的内存就不能被回收。所以malloc采用的是内存池的管理方式(ptmalloc)，ptmalloc采用边界标记法将内存划分成很多块，从而对内存的分配和回收进行管理。为了内存分配函数malloc的高效性，ptmalloc会预先向操作系统申请一块内存供用户使用，当我们申请和释放内存的时候，ptmalloc会将这些内存管理起来，并通过一些策略来判断是否将其回收给操作系统。  
    为了有效管理堆内存，glibc对堆内存进行缓存并提供申请释放接口malloc/free，所以系统申请释放内存都是调用malloc/free函数，而malloc最终通过操作系统的两个不同系统调用brk和mmap来实现。glibc的malloc实现中，充分考虑了brk和mmap行为上的差异，默认分配大块内存(128K)才使用mmap获得地址空间，也可通过mallopt(M_MMAP_THRESHOLD,size)来修改这个临界值。  
    系统(glibc)分配的内存均带有malloc_chunk首部。  

    ```C
    struct malloc_chunk
    {
        INTERNAL_SIZE_T prev_size;
        INTERNAL_SIZE_T size;

        struct malloc_chunk *fd;
        struct malloc_chunk *bk;

        struct malloc_chunk *fd_nextsize;
        struct malloc_chunk *bk_nextsize;
    }
    ```

    实际上在分配给用户的地址前边还有8个字节分别表示prev_size、size，fd以后内容在未释放的情况下，与返回给用户的内存空间重叠，仅在free之后使用。  
    glibc库源码中是malloc分配的内存按照MALLOC_ALIGNMENT宏来字节对齐的，在32位系统中MALLOC_ALIGNMENT为8，64位系统中MALLOC_ALIGNMENT为16。因此使用size字段低3个bit位分别表示不同的含义。  

    ```C
    #define PREV_INUSE 0x01  //当前chunk的使用状态
    #define IS_MMAPPED 0x02  //1表示mmap映射区域分配，0表示heap区域分配
    #define NON_MAIN_ARENA 0x04  //0为主分配区分配，1为非主分配区分配
    ```

    当用户使用free函数释放内存，glibc并不会马上交还给操作系统，而是被glibc本身的空闲链表bins一个u管理起来，这样当下次进程需要malloc这一块内存的时候，glibc就会从空闲的bins上寻找一块合适大小的内存块分配给用户使用。这样的好处可以避免频繁的系统调用，降低内存分配的开销。  

    malloc将相似大小的chunk用双向链表链接起来，这样一个链表被称为一个bin。系统一共维护了128个bins。每个bin都维护了大小相近的双向链表的chunk。基于chunk的大小，有下列几种可用bins:  

    1. Fast bin
    2. Unsorted bin
    3. Small bin
    4. Large bin

    保存这些bin的数据结构为:  

    * fastbinsY:这个数组用以保存fast bins。
    * bins：这个数组用以保存unsorted、small以及large bins，共计可容纳126个。  

    当用户调用malloc的时候，能很快找到用户需要分配的内存大小是否在维护的bin上，如果在某一个bin上，就可以通过双向链表去查找合适的chunk内存块给用户使用。  

    并不是所有chunk都按照上面的方式来组织，有三种列外情况。top chunk，mmaped chunk，last remainder chunk。  

4. Linux栈空间  
栈是程序运行的重要组成部分，是在程序运行过程中需要保存部分临时变量以及函数调用过程而开辟的内存空间。栈内存储的数据包含:局部变量、临时寄存器的值、上层函数返回地址、栈帧、函数参数等。  
栈上的内存用户不能直接申请，编译器根据用户定义的局部变量大小自动分配空间，一旦函数返回后，局部变量占用的内存会被自动回收，所以函数返回的地址不能是栈上的内存地址。栈的分配与释放过程如下:  

* 形成栈帧结构  
    在函数入口会先保存前一个函数的esp，例如push %ebp;mov %esp,%ebp;
* 分配栈空间  
    在函数形成栈帧结构后，如果有必要，会为函数分配栈空间用来存储局部变量或者临时寄存器，由于只有一个寄存器esp指向栈顶，所以申请空间也就是减小esp的值，释放栈空间也就是增加esp的值，另外push和pop可以每次申请释放一个内存单元。
* 参数入栈  
    函数入参传递包括寄存器传参和压栈传参以及两者共用。另外，入参传递顺序上，标准c语言是从右向左的。寄存器传参，在call func语句之前需要准备入参，把参数放到规定的寄存器中即可，mov xxx,%esi;语句。压栈传参，在call func语句之前需要准备好入参，按照参数从最后一个开始入栈，push $arg n;
* 函数返回地址入栈  
    函数入栈后，需要通过call指令调用子函数，实际上call指令隐藏了一个push指令，先把call的下一条指令压栈，也就是通过子函数返回后执行的下一条指令压栈，然后把call的函数地址放到eip中，完成函数的调用。  
* 函数返回  
    函数执行完后，需要销毁本函数的栈帧解构，以及执行当前函数调用前的下一条指令，要完成这两个过程，函数最后通过leave指令和ret指令来完成的。leave销毁栈帧，ret把函数返回地址取出放到eip寄存器中。leave指令实际上是销毁栈帧，相当于栈帧形成的逆过程，也就是move %ebp，%esp;pop %ebp;ret指令也就是把函数返回地址取出来放到eip寄存器中，相当于pop %eip。  

5. Linux段错误  
Segmentation Fault in Linux也就是linux中的SIGSEGV信号，和内存息息相关。  
    
* SIGSEGV是在访问内存时发生的错误，它属于内存管理的范畴。  
* SIGSEGV是一个用户态的概念，是操作系统在用户态程序错误访问内存时所做出的处理。  
* 当用户态程序访问不允许的内存时，产生SIGSEGV。  

从用户态程序开发的角度，我们并不需要理解操作系统复杂的内存管理机制，这是和硬件平台相关的，但是了解内核发送SIGSEGV信号的流程还是挺重要的。  

>-----------------------------|=是=>访问类型是否符合=是=>访问合法，分配内存  
>page fault==>地址是否属于进程地址------------------|===>非法访问，发送segfault信号  
>-----------------------------|=否=>异常发生在用户态=否=>内核错误  

segfault流程是操作系统完成的，应用程序通常是注册一个信号处理函数，操作系统会把segfault的信息以信号的形式发送给应用程序，应用程序通过解析信号携带的信息，来获取发送segfault的环境信息，信号的安装通过结构体struct sigaction来完成。  

```C
struct sigaction
{
    union
    {
        __sighandler_t _sa_handler;
        void(*_sa_sigaction)(int,struct siginfo *,void *);
    }_u;
    sigset_t sa_mask;
    unsigned long sa_flags;
}
```

信号回调函数携带的信息如下：  

```C
typedef struct siginfo
{
    int si_signo;
    int si_errno;
    int si_code;

    union
    {
        int _pad[SI_PAD_SIZE];

        /*kill()*/
        struct
        {
            __kernel_pid_t _pid; /* sender's pid */
            __ARCH_SI_UID_T _uid; /* sender's uid */
        } _kill;

        /* POSIX.1b timers */
        struct
        {
            __kernel_timer_t _tid; /* timer id */
            int _overrun; /* overrun count */
            char _pad[sizeof(__ARCH_SI_UID_T) - sizeof(int)];
            sigval_t _sigval; /*same as below */
            int _sys_private; /* not to be password to user */
        }

        /* POSIX.1b signals */
        struct
        {
            __kernel_pid_t _pid; /* sender's pid */
            __ARCH_SI_UID_T _uid; /* sender's uid */
            sigval_t _sigval;
        } _rt;

        /* SIGCHLD */
        struct
        {
            __kernel_pid_t _pid; /* which child */
            __ARCH_SI_UID_T _uid; /* sender's uid */
            int _status; /* exit code */
            __kernel_clock_t _utime;
            __kernel_clock_t _stime;
        } _sigchld;

        /* SIGILL,SIGFPE,SIGSEGV,SIGBUS */
        struct
        {
            void __user *_addr; /* faulting insn/memory ref. */
            short _addr_lsb; /* LSB of the reported address */
        } _sigfault;

        /* SIGPOLL */
        struct
        {
            __ARCH_SI_BAND_T _band; /* POLL_IN,POLL_OUT,POLL_MSG */
            int _fd;
        } _sigpoll;
    } _sifields;
} _siginfo_t;
```

si_code不同的值表示不同的产生原因，其中含义如下：  

```C
/*
* SIGSEGV si_codes
*/
#define SEGV_MAPERR 1 /* address not mapped to object */
#define SEGV_ACCERR 2 /* invalid permissions for mapped object */
```

6. Linux内存相关命令  

+ 查看物理内存信息  
    ***free -m***查看系统物理内存统计情况
+ 查看各个进程内存使用信息  
    ***top***命令可以查看每个进程实时占用的内存，其中VIRT表示每个进程占用的虚拟内存大小，RES表示每个进程实际映射的物理内存大小，SHR表示每个进程占用的共享内存大小。  
+ 查看进程虚拟内存空间  
     1. pmap命令  
        ***pmap -x pid***，Linux通过map表记录了每个进程物理内存和虚拟内存的映射关系，其中第一列表示虚拟地址，第二列Kbytes表示虚拟内存大小，第三列RSS表示虚拟内存映射的物理内存大小，由于Linux延时映射的原理，当用户申请内存的时候，实际上虚拟内存并没有和物理内存映射，当用户对该内存进行读写操作的时候，才会进行虚拟内存和物理内存映射。  
     2. maps文件  
        /proc/pid/maps:显式了进程映射的内存区域和访问权限。

        >00-02 r-xp 00000000 00:00 0 [vdso]
        >虚拟地址空间起始地址-虚拟地址空间结束地址 地址空间属性 偏移 设备号 节点号 文件名  
     3. smaps文件  
        /proc/pid/smaps文件系统可以查看进程更详细的内存空间信息，smaps文件和maps文件显示的信息类似。  
     4. /proc/meminfo
        meminfo是Linux系统内存统计的总体信息。  

7. 内存相关问题定位  
内存方面有两个常用工具Valgrind和Address Sanitizer(简称ASan)。Valgrind是强大开源的程序检测工具，不需要重新编译。ASan比Valgrind更高效，gcc 4.8开始，ASan成为gcc的一部分，但不支持符号信息，无法显示出问题的函数和行数。从4.9开始，gcc支持ASan的所有功能。在用gcc编译程序时，指定-fsanitize=address选项即可自动调用ASan，运行程序时，就可以看到相关信息。  

+ 内存泄漏  
    内存泄漏是指因为忽略或错误造成程序未能释放已经不再使用的内存的情况。内存泄漏会引起申请内存失败或者系统的OOM killer。  
    内存泄漏的常用定位放法有内存统计、mtrace、valgrind、ASan等。  
    内存统计方法最简单，但是需要提前在内存申请和释放的地方添加统计，这种方法只能用于有内存统计功能的情况。  
    mtrace可以跟踪各种malloc、calloc、realloc、free的调用，并将调用记录存储到指定的文件中，然后通过mtrace工具即可分析是否有内存未被释放。mtrace原理是记录每一对malloc、free的执行，若每一个malloc都有相应的free，则代表没有内存泄漏，对于任何非malloc/free情况下所发生的内存泄漏问题，mtrace并不能找出来。  
    大型平台下一般不使用mtrace来定位内存泄漏。mtrace一般用于定位某一段内存释放是否有内存泄漏，在内存分配调用之前，调用mtrace()函数，在想结束分析的代码位置调用muntrace()。      
+ OOM Killer  
    Linux内存分为虚拟内存和物理内存，应用程序的内存空间都是虚拟内存，Linux为了提高内存的利用率，用户申请内存的时候，直接返回给用户一个虚拟地址，该虚拟地址并不映射物理内存，在用户实际读写该内存地址的时候，才会触发物理内存的映射，也就是分配物理内存。  
    Linux这种内存管理的机制在大多数情况下是没有问题的，但是当物理内存远远小于用户使用的内存的时候就会出现问题;如果某几个进程在某一时刻使用的物理内存非常多，已经远远超过物理内存大小了，而且还在请求分页，这个时候内核已经没有多余的物理内存来满足应用进程的分页请求了，为了能够完成自己的承诺，于是内核就不得不根据一定的规则杀掉某些进程来完成自己的承诺，这就是OOM killer(Out-Of-Memory killer)。  
    发现进程被莫名其妙的杀掉情况，可以通过dmesg查看内核日志有没有oom_killer类似的信息。  
+ 内存碎片化  
    内存碎片化通常分为内部碎片和外部碎片：  

    1. 内部碎片是由于采用固定大小的内存分区，当一个进程不能完全使用分给它的固定内存区域时就产生了内部碎片，通常内部碎片难以完全避免。  
    2. 外部碎片是由于某些未分配的连续内存区域太小，以至于不能满足任意进程的内存分配请求，从而不能被进程利用的内存区域。  
    
    现在普遍采用的段页式内存分配方式就是将进程的内存区域分为不同的段，然后将每一段由多个固定大小的页组成。通过页表机制，使段内的页可以不必连续处于同一内存区域，从而减少了外部碎片，然而同一页内仍然可能存在少量的内部碎片，只是一页的内存空间本就较小，从而使可能存在的内部碎片也较少。  

+ 内存越界  
    内存是连续的空间，每次申请内存的时候都会传入一个大小，然后根据这个大小分配一个合适的大小(内存对齐原则)空间，然后这个内存块可以给用户使用每个内存块前面都有一个内存头用于内存管理。若写内存越界就会把后面的内存头信息覆盖，申请或者释放后面紧邻内存块时就会出错导致宕机。  
    通常的定位方法有：加guard字节、gdb调试方法、设置页保护属性、Valgrind、ASan，通常使用加guard字节方法。  
    1. 加guard字节方法  
        加guard字节方法是在内存尾部紧接添加一个guard字节，释放的时候检查这个guard字节是否被改写，如果改写就认为发生了内存越界。这种方法存在一个缺陷就是在内存释放的时候才检测出来，如果没有记录内存申请者的信息，无法准确知道是甚么原因或操作导致内存越界，存在时延。
    2. gdb调试方法  
        gdb调试的方法，前提是需要知道被越界改写的指针地址，然后设置硬件断点(watch)。
    3. 设置页保护属性方法  
        设置页保护属性方法就是将内存尾部设置页保护属性，将内存属性设置为只可读不可写属性，这个时候一旦发生内存越界，则立即宕机，捕捉第一现场，可以准确知道内存越界的原因，没有时延，由于需要设置大量的页保护，存在浪费内存和影响性能的问题，一般只在复现定位问题时才使用。同时由于内存对齐，可能存在部分内存越界无法发现。  
    4. Valgrind方法  
        编译并使用Valgrind启动，可以检查出内存越界并打印出相应的信息。

        >valgrind --leak-check=full --show-reachable=yes --trace-childre=yes ./mem_overflow

    5. ASan方法  
        gcc编译的时候指定-fsanitize=address选项并执行。

        >gcc -g mem_overflow.c -fsanitize=address -o mem_overflow

+ 堆栈溢出  
   堆栈溢出主要是因为函数中的局部数组的越界错误操作引起的，堆栈溢出后，程序一样会出现跑飞。  
   * 栈帧保护  
        GCC4.1以上版本有三个与堆栈保护有关的编译选址  
        1. -fstack-protector:启用堆栈保护，不过只为局部变量中含有char数组的函数插入保护代码。  
        2. -fstack-protector-all:启用堆栈保护，为所有函数插入保护代码。  
        3. -fano-stack-protector：禁用堆栈保护。  
        启用堆栈保护选项后编译器会产生额外的代码来检测栈溢出，即在局部变量和指令指针之间(返回地址和栈帧之间)插入8个字节的魔术字，这个值被称作金丝雀("canary");出现栈溢出时，如果canary值被改变，检测机制就会触发程序挂起，并有"__stack_chk_fail()"提示字样。  
        局部变量越界引入的段错误现象比较多，因为局部变量数组越界的范围不清楚，可能改写栈上的任何数据，甚至有的越界连调用栈也无法找到，对于这种问题，抓住主线，寻找当前函数或者子函数中使用的局部数组，重点检查是否存在越界情况，即可找到问题的原因。  
   * ASan方法   
        ASan能准确检测出哪个局部变量越界。gcc指定-fsanitize=address选项开启ASan功能，执行程序发现堆栈越界，能找到发生越界的局部数组名“name”。同时只发生了一个字节的越界也能检测出来。如果不指定-fsanitize=address选项，输入8个1实际已经越界了，但是没有任何异常，让人误解为正常。  

+ 全局变量越界  
    由于全局变量的地址是在编译阶段确定的，所以在运行前全局变量的地址已经确定，而且全局变量前后一般也是全局变量，所以定位全局变量越界的问题可以通过符号表法，gdb硬件断点法，ASan方法等。  

    1. 符号表分析方法  
        通过符号表查找该全局变量前后的全局变量。  
        ***nm -n filename***找到被改写的符号表前后的全局变量。  
    2. gdb硬件断点方法  
        使用watch增加硬件断点。
    3. ASan方法  
        gcc编译时使用-fsanitize=address选项，ASan同样可以检测出全局变量越界。

### 2. procfs文件系统  
1. 简介  
Linux procfs是一个虚拟文件系统，通常挂载在/proc目录下，提供了以文件读写方式访问内核信息的接口。  
procfs中的文件大多是只读的，用于导出内核信息，也有一些文件是可写的，用于对内核进行配置。  
因为procfs中的文件以文本的形式呈现内容，所以可以方便的访问。一般使用cat/more/less等shell命令就可以读取其中的内容，使用echo等shell命令就可以对其进行写入(echo value > /proc/file)。  
proc即process information pseudo-filesystem，procfs最初主要用于导出系统中的进程信息，目前进程信息也是procfs中的主要内容，但procfs中也包含其他的内核信息。  

2. procfs浏览  

+ 进程信息  
    procfs下以数字命名的目录包含进程信息，目录名的数字表示进程的pid。  
    /proc/self是一个软链接，指向当前进程，为访问当前进程信息提供便捷方式。  

    进程目录下的主要目录和文件介绍如下：  

    - /proc/[pid]/attr  
        此目录与内核中的安全模块相关，可以通过对文件进行读写的方式进行安全相关属性的获取和配置，此目录的引入是为了支持selinux，但也可以支持其它的内核安全模块。  
    - /proc/[pid]/autogroup  
        此目录与进程调度相关。
    - /proc/[pid]/clear_refs  
        这是一个可写的文件，用于重置进程关联的内存页面的情况。  
    - /proc/[pid]/cmdline  
        进程完整的命令行，对于僵尸进程，则为空。使用strings查看可读性更好。
    - /proc/[pid]/comm  
        进程名，可以通过写入此文件改变进程名，进程下的不同线程(/proc/[pid]/task/[tid]/comm)可以有不同的线程名。  
    - /proc/[pid]/coredump_filter  
        控制coredump发生时，进程的哪些内存页面被写入core文件，此文件可读可写，文件内容按位掩码理解。
    - /proc/[pid]/cwd  
        指向进程当前工作目录的软链接。  
    - /proc/[pid]/environ  
        进程的环境变量，使用strings命令读取，输出格式可读性更好。  
    - /proc/[pid]/exe  
        软链接，执行进程对应的可执行文件的实际路径。  
    - /proc/[pid]/fd/  
        目录，其中包含若干个软链接，软链接以数字命名，对应进程打开的文件描述符，软链接指向进程所打开的文件路径，对于特殊文件，文件路径的格式为type:[inode]，常见的特殊文件有：  
        管道:pipe：[inode]
        套接字:socket：[inode]，可通过inode在/proc/net/中找到该套接字相关的具体信息。
    - /proc/[pid]/fdinfo/  
        目录，其中包含若干个文件，文件以数字命名，对应进程打开的文件描述符，文件的内容为文件描述符的具体信息，对于常规文件和目录，其中的字段如下:  

        |字段|说明|  
        |:--:|:--:|  
        |pos|文件描述符的当前偏移|  
        |flag|文件描述符的flag和mode|  
        |mnt_id|文件所在文件系统，参考/proc/[pid]/mountinfo|  

    - /proc/[pid]/io  
        进程的IO统计。  

        |字段|说明|
        |:--:|:--:|
        |rchar|进程读取的字节数|
        |wchar|进程写入的字节数|
        |syscr|进程调用的读取操作的系统调用总数|
        |syscw|进程调用的写入操作的系统调用总数|
        |read_bytes|进程从存储层(挂载文件系统的块设备)读取的字节数|
        |write_bytes|进程向存储层写入的字节数|
        |cancelled_write_bytes|进程由于截断pagecache导致应该发生而没有发生的写入字节数|

    - /proc/[pid]/limits  
        进程的系统资源限制，包括CPU、文件、内存、信号等。  
    - /proc/[pid]/map_files/  
        映射到进程地址空间的文件信息，这是一个目录，目录中包含若干个软链接，软链接的名字表示映射的地址空间范围，软链接指向映射对应的文件路径。  
    - /proc/[pid]/maps  
        进程的地址空间映射情况。
    - /proc/[pid]/mem  
        可以通过此文件访问进程的内存，一般用作调试用途。
    - /proc/[pid]/mountinfo  
        进程的文件系统挂载信息，每一行表示一个文件系统。  
    - /proc/[pid]/mounts  
        进程的文件系统挂载信息，每一行表示一个文件系统，与mountinfo类似，但可读性更好。
    - /proc/[pid]/mountstats  
        进程的文件系统挂载信息，但信息更精简，仅包括设备、挂载点，文件系统类型。  
    - /proc/[pid]/net  
        网络资源相关的信息，随着namespaces的引入，进程的网络资源被隔离在Network namespace中，因此进程的网络资源相关的信息，被放在了/proc/[pid]/net，而原本的/proc/net变成了指向/proc/self/net的软链接。  
    - /proc/[pid]/oom_adj  
    - /proc/[pid]/oom_score  
    - /proc/[pid]/oom_score_adj  
        进程在out-of-memory(OOM)系统中的评分相关信息，当系统剩余内存不足时，会根据评分选择进程结束，评分越大的进程，系统越倾向于结束，评分的主要依据是进程的内存占用情况，一般内存占用大的进程，容易被系统结束。对于不希望被系统结束的进程，可以通过这些文件进行配置。  
    - /proc/[pid]/root  
        软链接，指向进程的根目录在文件系统中的路径。
    - /proc/[pid]/smaps  
        进程地址空间中，每一段内存映射消耗的内存情况
    - /proc/[pid]/stack  
        进程的内核态栈信息。内核配置中需要打开CONFIG_STACKTRACE。进程的用户态栈和内核态栈是独立的，进程通过系统调用进入内核态执行时使用内核态栈。  
    - /proc/[pid]/status  
        进程的综合信息。  
    - /proc/[pid]/task  
        进程的线程信息，这个目录包含多个以数字命名的子目录，每个子目录代表此进程中的一个线程，数字表示线程ID，目录中的内容与这里介绍的/proc/[pid]/基本一致。  
+ 其它内核信息  
    Linux procfs除包含进程信息外，还包含一些其它内核信息。  

    * /proc/devices  
        系统中已注册的字符设备和块设备信息，包含设备号。当驱动程序加载成功后，可以通过检查这个文件判断设备是否已成功注册，通过查看分配到的设备号判断设备文件的设备号是否正确。  
    * /proc/filesystems  
        当前内核支持的所有文件系统。如果文件系统被标记为'nodev'，表示这种文件系统不需要对应的存储设备。  
    * /proc/interrupts  
        中断相关的信息。  
    * /proc/irq  
        中断相关的目录，该目录下又有多个以中断号命名的子目录(对应/proc/interrupts)。在多核处理器系统中，每个子目录中有两个文件smp_affinity和smp_affinity_list，可以通过读取和写入对中断的CPU亲和性进行配置。其中，smp_affinity以掩码的形式表示CPU亲和性，smp_affinity_list以列表的形式表示CPU亲和性，读取和设置其中任何一个文件都是等价的。进行性能优化时，可以通过设置中断的亲和性将中断绑定在指定的CPU核心上进行处理。  
    * /proc/iomem  
        I/O内存的分配情况。驱动程序一般会通过request_mem_region()函数对需要访问的IO内存进行分配，可以通过此文件确认分配是否成功，以及是否存在IO内存已被占用的情况。  
    * /proc/kallsyms  
        内核的符号和地址信息(地址信息需要root权限才能查看)，调试时可以通过地址信息直接访问相关的符号。  
    * /proc/locks  
        当前系统中关于文件锁的信息，文件中每一行表示一个文件锁。  
    * /proc/meminfo  
        内存相关的信息。  
    * /proc/modules  
        内核中已经加载的模块信息，通过lsmod(8)查看，可读性更好。  
    * /proc/mounts  
        挂载的文件系统信息，这里是指向/proc/self/mounts的软链接，具体信息查看/proc/[pid]/mounts
    * /proc/slab  
        slab内存信息，具体情况查看slabinfo。
    * /proc/stat  
        CPU为CPU时间统计，CPU%d为按核心的CPU时间统计，单位为USER_HZ(sysconf(_SC_CLK_TCK))。  

3. procfs编程实例  
procfs可以扩展，通过新增procfs文件，或者修改已有的proc文件，用于实现内核态的配置和调试功能。  

+ NAND Flash操作统计  
    本编程实例的背景是需要通过对NAND Flash操作进行统计，从而评估在产品应用中器件失效的风险。  
    具体来说，在NAND Flash驱动层按block统计read page和erase block的次数，通过procfs文件读取，输出统计值，通过procfs文件写入，控制输出的行为，包括：  
    - 设置read的显式阈值(小于此值不显示)  
    - 设置erase的显式阈值(小于此值不显示)  
    - 清空read操作计数  
    - 清空erase操作计数  

    1. procfs文件创建  
        + 包含必要的内核头文件:<linux/procfs.h>和<linux/seq_file.h>  
        + 初始化函数通过later_initcall()在内核初始化期间被执行  
        + 通过proc_create(arg1,arg2)创建所需procfs文件：arg1为字符串形式的procfs文件路径，arg2为struct file_operations结构体，定义了所创建procfs文件的操作。  
        ```C
        #include <linux/proc_fs.h>
        #include <linux/seq_file.h>

        static int fhdrv_add_flash_cnt()
        {
            struct proc_dir_entry *res = NULL;

            fhdrv_kdrv_get_flash_cnt_tbl(&g_flash_cnt_info);
            if(NULL == fhdrv_kdrv_get_flash_cnt_tbl)
            {
                printk(KERN_ERR"Failed to get flash_cnt info.\n");
                return -ENOME;
            }
            res = proc_create("driver/fhdrv_kdrv_flash_cnt",fhdrv_flash_proc_func);

            if(NULL == res)
            {
                printk(KERN_ERR"Failed to create fhdrv_kdrv_flash_cnt.\n");
                return -ENOME;
            }

            return 0;
        }
        late_initcall(fhdrv_kdrv_add_flash_cnt);
        ```

    2. struct file_operations  
        + procfs文件的操作通过struct file_operations结构体定义，使用seq_file接口。  
        + seq_read()、seq_lseek()、single_release()都是内核提供的标准实现，直接绑定到read、lseek、release方法即可。  
        + open方法对应procfs文件的打开，绑定到函数fhdrv_flash_cnt_proc_open()，其实现为调用内核提供的标准实现single_open()，将打开的文件绑定到函数fhdrv_flash_cnt_proc_show()，此函数将对应文件的显式。  
        + write方法对应procfs文件的写入，绑定到函数fhdrv_flash_cnt_proc_write()。  

        ```C
        static int fhdrv_flash_cnt_proc_open(struct inode *inode,struct file *file)
        {
            return single_open(file,fhdrv_flash_cnt_proc_show,NULL);
        }

        static struct file_operations fhdrv_flash_proc_func = 
        {
            .owner = THIS_MODULE,
            .open = fhdrv_flash_cnt_proc_open,
            .read = seq_read,
            .write = fhdrv_flash_cnt_proc_write,
            .llseek = seq_lseek,
            .release = single_release,
        };
        ```

    3. procfs文件读取  
        + fhdrv_flash_cnt_proc_show()函数对应procfs文件的读取。  
        + 通过函数seq_printf()输出，其用法类似printf()。  
        + 在函数fhdrv_flash_cnt_proc_show()中，可以对输出的内容进行控制和格式化，在此例中，包含辅助信息的输出，以及通过设置阈值对输出进行过滤。  
        ```C
        static int fhdrv_flash_cnt_proc_show(struct seq_file *seq,void *v)
        {
            int i = 0;
            struct timespec now;

            fhdrv_get_uptime(&now);
            seq_printf(seq,"\nNand block read cnt show limit = %u\n", g_flash_cnt_info.read_show_limit);
            seq_printf(seq,"Nand block erase cnt show limit = %u\n",g_flash_cnt_info.erase_show_limit);
            seq_printf(seq,"Nand block read cnt clear timestamp = %lu Sec\n",(unsigned long)g_flash_cnt_info.rc_stamp.tv_sec);
            seq_printf(seq,"Nand block erase cnt clear timestamp = %lu Sec\n",(unsigned long)g_flash_cnt_info.ec_stamp.tv_sec);
            seq_printf(seq,"uptime = %lu Sec\n",(unsigned long)now.tv_sec);
            seq_printf(seq,"Block\tRead\tErase\t\t\n");
            for(i = 0; i < g_flash_cnt_info.tbl_size; i++)
            {
                if(g_flash_cnt_info.read_cnt_tbl[i] < g_flash_cnt_info.read_show_limit)
                    continue;
                if(g_flash_cnt_info.erase_cnt_tbl[i] < g_flash_cnt_info.erase_show_limit)
                    continue;
                if((0 == g_flash_cnt_info.read_cnt_tbl[i]) && 
                    0 == g_flash_cnt_info.erase_cnt_tbl[i]))
                    continue;
                seq_printf(seq,"%d\t%u\t%u\n",i,g_flash_cnt_info.read_cnt_tbl[i],g_flash_cnt_info_erase_cnt_tbl[i]);
            }
            return 0;
        }
        ```
    4. procfs文件写入  
        + fhdrv_flash_cnt_proc_write()函数对应procfs文件的写入。  
        + procfs文件的写入一般用来实现简单的控制命令。  
        + 写入procfs文件的数据来自用户态，因此buff不能直接使用，本例中通过内核函数copy_from_user()访问。  
        + 对于写入的数据，通过函数fhdrv_handle_cmd()函数解析并处理。  
        + 失败时返回出错码，成功时返回写入的字节数(len)。  

        ```C
        static ssize_t fhdrv_kdrv_flash_cnt_proc_write(struct file *file,const char *buff,size_t len,loff_t *offset)
        {
            char input[PROC_CMD_MAX_LEN] = {0};
            if(len >= PROC_CMD_MAX_LEN)
            {
                return -EINVAL;
            }
            if(copy_from_user(input,buff,len) != 0)
            {
                return -EFAULT;
            }
            if(0 != fhdrv_handle_cmd(input))
            {
                return -EINVAL;
            }

            return len;

        }
        ```
4. sysctl与sysfs  

+ sysctl简介  
    与一般的procfs文件通常只用于读取不同，/proc/sys目录下通常为可以配置的内核参数，即可写。  
    除了直接写入/proc/sys目录中的文件进行配置外，还有一个专用的配置程序(sysctl)。  
    /proc/sys/目录中的内容如下表：  

    |目录|说明|
    |:--:|:--:|
    |abi|abi相关信息，参考内核文档abi.txt|
    |debug|debug相关信息，和具体系统相关|
    |dev|设备相关信息，如dev/cdrom/info|
    |fs|文件系统相关信息，参考内核文档fs.txt|
    |kernel|内核的通用信息，参考内核文档kernel.txt|
    |net|网络子系统相关的信息，参考内核文档net.txt|
    |user|与用户态相关的信息，如cgroup、inotify、ipc等|
    |vm|内存子系统相关的信息，参考内核文档vm.txt|

+ sysctl编程接口  
    sysctl的编程接口，相比procfs更简单，以/proc/sys/net为例。  
    - 定义ctl_table数据结构，其中包含多个需要导出的内核变量，每个变量又包括:对应的procfs文件路径、变量、长度(类型)、权限，内核预定义的handler。  
    - 通过函数register_net_sysctl()注册ctl_table结构体。  
    - 与seq_file接口可以实现较为复杂的控制和格式化相比，sysctl将内核数据结构作为基本数据类型导出，使用更加方便。  

    ```C
    static struct ctl_table ipv4_table[] = {
        {
            .procname = "tcp_timestamps",
            .data = &sysctl_tcp_timestamps,
            .maxlen = sizeof(int),
            .mode = 0644,
            .proc_handler = proc_dointvec
        },
        {
            .procname = "tcp_window_scaling",
            .data = &sysctl_tcp_window_scaling,
            .maxlen = sizeof(int),
            .mode = 0644,
            .proc_handler = proc_dointvec
        }
    }

    static __init int sysctl_ipv4_init()
    {
        struct ctl_table_header *hdr,
        hdr = register_net_sysctl(&init_net,"net/ipv4",ipv4_table);
        if(!hdr)
            return -ENOMEM;
        if(register_pernet_subsys(&ipv4_sysctl_ops))
        {
            unregister_net_sysctl_table(hdr);
            return -ENOMEM;
        }
        return 0;
    }
    __initcall(sysctl_ipv4_init);
    ```
+ sysfs简介  
    与sysctl类似，sysfs也是用于导出内核数据结构信息的内存文件系统。  
    sysfs是内核kobject机制的体现，kobject对象对应sysfs目录，kobject对象间的关系对应sysfs的目录结构。  
    sysfs是内核ABI的一部分，是稳定而清晰的。  

    |目录|说明|
    |:--:|:--:|
    |block|块设备相关的目录，包含指向/sys/devices目录下对应块设备的软链接|
    |bus|按总线分类的设备目录，例如pci、usb，包含指向/sys/devices目录下对应设备的软链接|
    |class|按类型分类的设备目录，如：input、tty，其中包含指向/sys/devices目录下对应设备的软链接|
    |dev|包含block和char两个子目录，及以设备号命名且指向/sys/devices目录下对应设备的软链接|
    |devices|设备信息，是内核设备树的映射，此处包含实际的设备信息|
    |fs|文件系统相关的信息|
    |module|内核模块相关的信息，在parameters目录下可以查看或者该模块参数|
    
+ procfs、sysctl、sysfs的比较  
    procfs、sysctl、sysfs都是用来导出内核信息的内存文件系统，但其各有特点：  

    |文件系统|特点|
    |procfs|主要为导出内核中的进程信息而设计，多数文件为只读，文件内容一般较为丰富，在文件的读取和写入时一般实现了较多的格式化和解析操作，常用于查看进程和其他内核信息|
    |sysctl|主要为导出内核中的配置参数而设计，多数文件可读可写，文件内容一般对应基本的数据结构，比较短小和简单，常用于查看和配置内核参数|
    |sysfs|主要为导出内核中的设备信息而设计，内核kobject机制的体现，内核ABI的一部分，是稳定而清晰的，常用于查看和配置设备的相关信息|  

### 3. Linux进程管理  
1. Linux进程概述  
Linux进程的基本特性如下：  
* Linux是多任务多用户操作系统，一个任务(task)就是一个进程(process)。  
* 每一个进程都具有一定的功能和权限，它们都运行在各自独立的虚拟地址空间。  
* 在Linux中，进程是系统资源分配的基本单位，也是CPU运行的基本调度单位。  
* 进程在OS内用PCB(process control Block)，PCB是进程的属性之一。  

+ 进程即及程基本定义  
    进程与线程是程序运行的基本单位，Linux下进程与线程的定义如下：  
    进程(process)：进程是程序(可执行程序代码)的一次执行及其所包含资源(打开文件、挂起信号、地址空间、数据段等)的总称。  
    线程(thread)：线程是进程中活动的对象，有独立的程序计数器、进程栈及一组进程寄存器。  
+ 进程与程序的区别  
    进程是一段功能完整的程序在处理机上的执行过程:  
    - 进程是动态的，是实时变化的，程序是静态的  
    - 进程是暂时的概念，相对而言，程序是永久的概念  
    - 进程有自己的数据结构(PCB)  
    - 进程和程序不是一一对应的  
+ 进程与线程的区别  
    - 从形态角度：  
        一个进程可包含一个或多个线程  
    - 从调度角度：  
        1. 进程是资源分配的基本单位  
        2. 线程是处理器调度的独立单位  
    - 从虚拟化角度：  
        1. 进程提供两种虚拟机制：虚拟处理器(进程独享处理器的假象)、虚拟内存(进程拥有系统内所有内存资源的假象)  
        2. 线程之间可共享虚拟内存，但各自拥有独立虚拟处理器  
+ 进程构成要素  
    进程的构成要素包括正文段，进程数据段，系统堆栈三部分。  
    - 正文段：  
        存放进程运行的代码，描述进程需完成的功能  
    - 进程数据段：  
        1. 存放正文段在执行期间所需的数据和工作区包括全局变量、动态分配的空间(malloc函数)  
        2. 用户栈也在该数据段开辟，存放函数调用时的栈帧、局部变量等  
    - 系统堆栈：  
        1. 每个进程捆绑一个，进程在内核态下工作时使用  
        2. 保存中断现场、执行函数调用时的参数和返回地址等  
        3. 其中最重要的数据结构是进程控制块  

2. 进程虚拟地址结构  
进程的虚拟地址空间包括用户空间和内核空间，以32位Linux系统为例(共4G空间):  

    * 用户空间(0x0000 0000-0xbfff ffff)  
        用户空间包括进程的可执行映像，进程运行时堆栈进程控制信息：  
        1. 可执行映像  
        2. 进程运行时堆栈  
        3. 进程控制信息，如进程控制块  
    * 内核空间(0xc000 0000-)  
        1. 内核被映射进程内核空间  
        2. 只允许进程在核心态下访问  

+ Linux进程描述符  
    Linux内核利用数据结构task_struct来描述，它代表一个进程的PCB。task_struct定义于include/linux/sched.h中，内核以此感知进程的存在，每当建立新进程的时候，Linux为新的进程分配一个task_struct结构。进程描述符提供了内核所需了解的进程信息，它是内核进行数据管理的核心参数。  
    * 进程描述符的基本信息如下:  
        1. 数据结构：task_struct  
        2. 定义位置：include/linux/sched.h  
        3. include/linux/sched.h中的struct task_struct  
    * 进程描述符向量结构：  
        1. 数据结构：task[NR_TASKS]  
        2. 定义位置：include/linux/sched.h  
        3. 定义格式：  
        ```C
        struct task_struct *task[NR_TASKS] = {&init_task}
        #define NR_TASKS 512
        ```
        4. 说明  
            全局变量NR_TASKS记录系统可容纳进程数，默认大小是512  
+ Linux进程描述符的信息组成  
    Linux的进程描述信息包括:  
    1. 进程状态信息(state,flags,ptrace)  
    2. 调度信息(static_prio,normal_proi,run_list,array,policy)  
    3. 内存管理(mm,active_mm)  
    4. 进程状态位信息(binfmt,exit_state,exit_code,exit_signal)  
    5. 身份信息(pid,tgid,uid,suid,fsuid,gid,egid,sgid,fsgid)  
    6. 家族信息(real_parent,parent,children,sibling)  
    7. 进程耗时信息(realtime,utime,stime,starttime)  
    8. 时钟信息(it_prof_expires,it_virt_expires,it_sched_expires)  
    9. 文件系统信息(link_count,fs,files)  
    10. IPC信息(sysvsem,signal,sighand,blocked,sigmask,pending)  

3. 进程的状态  
task_struct中的state表示进程当前的状态，它的定义如下：volatile long state  
state成员变量表征进程的可运行性，具体枚举信息见[include/linux/sched.h]  

+ Linux进程状态说明  
    Linux的运行状态，主要包括运行态，挂起态以及不可运行态。  

    1. 运行态/就绪态  
        * TASK_RUNNING：正在运行或已处于就绪只等待CPU调度  
        * TASK_TRACED：供调试使用  
    2. 被挂起状态  
        * TASK_INTERRUPTIBLE：可被信号或中断唤醒进入就绪队列  
        * TASK_UNINTERRUPTIBLE：等待资源，不可被其他进程中断  
        * TASK_STOPPED：被调试暂停，或收到SIGSTOP等信号  
    3. 不可运行态  
        * TASK_ZOMBIE：正在终止(已释放内存、文件等资源，但内核数据结构信息未释放)，等待父进程通过wait4()或waitpid()回收  
        * TASK_DEAD：已退出且不需要父进程回收的进程状态  
+ 进程状态转换图  
    Linux的进程调度器，会控制进程的运行状态，在各个状态下切换，主要是处理运行态和挂起态的切换。进程状态切换通过相关函数完成，具体函数以及状态切换信息如下：  

    1. 1TASK_RUNNING->TASK_UNINTERRUPTIBLE  
        函数名：sleep_on()
        * 拥有CPU的进程申请资源无效时，通过sleep_on()，将进程从TASK_RUNNING切换到TASK_UNINTERRUPTIBLE状态  
        * sleep_on()函数作用就是将current进程的状态置成TASK_UNINTERRUPTIBLE，并加到等待队列中  
        * 一般来说引起状态变成TASK_UNINTERRUPTIBLE的资源申请，都是对一些硬件资源的申请，如果得不到这些资源，进程将不能执行下去，不能由signal信号或时钟中断唤醒回到TASK_RUNNING状态。  
    2. TASK_RUNNING->TASK_INTERRUPTIBLE  
        函数名：interruptible_sleep_on():  
        * 拥有CPU的进程申请资源无效时，通过该函数将进程从TASK_RUNNING切换到TASK_INTERRUPTIBLE状态。  
        * interruptible_sleep_on()函数作用就是将current进程的状态置成TASK_INTERRUPTIBLE，并加到等待队列中。  
        * 处于TASK_INTERRUPTIBLE状态的进程可在资源有效时被wake_up()、wake_up_interruptible()或wake_up_process()唤醒，或收到signal信号以及时间中断后被唤醒。  
    3. TASK_RUNNING->TASK_UNINTERRUPTIBLE  
        函数名：sleep_on_timeout():  
        将进程从TASK_RUNNING->TASK_UNINTERRUPTIBLE  
    4. TASK_RUNNING->TASK_INTERRUPTIBLE  
        函数名：interruptible_sleep_on_timeout()：  
        虽然在申请资源或运行中出现了某种错误，但是系统仍然给进程一次重新运行的机会。调用该函数将进程从TASK_RUNNING切换到TASK_INTERRUPTIBLE状态，并等待规定的时间片长度，再重新试一次。  
    5. TASK_UNINTERRUPTIBLE->TASK_RUNNING/TASK_INTERRUPTIBLE->TASK_RUNNING  
        函数名：wake_up()  
        * 处于TASK_UNINTERRUPTIBLE状态的进程不能由signal信号或时钟中断唤醒，只能由wake_up()或wake_up_process()唤醒。  
        * wake_up()函数的作用是将wait_queue中所有状态为TASK_INTERRUPTIBLE或TASK_UNINTERRUPTIBLE的进程状态置为ASK_RUNNING，并将它们都放到running队列中去，即唤醒所有等待在该队列上的进程。  
+ 进程标识  
    进程标识在task_struct中的成员名为pid_t pid，功能是：  

    * 内核通过pid标识每个进程  
    * pid与进程描述符之间有严格的一一对应关系  
        pid_t实际上是一个int类型，默认取值范围为0-32767，如果需要调整这个范围，可以通过调整/proc/sys/kernel/pid_max中的参数实现;Linux通过get_pid()实现获取进程ID，每次新增线程，pid加1，循环递增。    

    1. 通过ps命令获取进程信息  
    2. /proc目录下的进程信息  
    3. 进程组标识  
        进程从属于具体的进程组，具体描述如下：  
        * 成员名：pid_t tgid  
        * 功能：  
            - 标识进程是否属于同组，组ID是第一个组内线程(父进程)的ID  
            - 线程组中的所有线程共享相同的PID  
        * 组ID赋值相关操作  
            - 单线程进程：tgid和pid相等  
            - 多线程进程：组内所有线程tgid都相等，且等于父进程pid  
    4. 用户相关的进程标识信息  
        每个用户组都有相关的进程，用来控制用户对系统资源的访问权限，具体说明如下：  
        * 功能：控制用户对系统资源的访问权限  
        * 分类  
            - 用户标识uid及组标识gid  
                通常是进程创建者的uid和gid  
            - 有效用户标识euid即有效组标识egid  
                有时系统会赋予一般用户暂时拥有root的uid和gid(作为用户进程的euid和egid)，以便于进行运作  
            - 备份用户标识suid及备份组标识sgid  
                在使用系统调用改变uid和gid时，利用其保留真正uid和gid  
            - 文件系统标识fsuid及文件系统组标识fsgid  
                检查对文件系统访问权限时使用，通常与euid及egid相等  
                在NSF文件系统中，NSF服务器需要作为一个特殊的进程访问文件，此时只修改客户进程的fsuid和fsgid，而不改变euid及egid，可避免受恶意攻击。  
            - 获取用户相关的进程标识信息的方法：getXXX()  
    5. Linux进程控制块的存放  
        Linux的进程控制的核心就是PCB，其存放的策略如下：  
        * Linux的进程控制块与所属进程的堆栈放在一起  
        * Linux为每个进程分配一个8KB大小的内存区域，用于存放该进程两个不同的数据结构：  
            - 进程描述符task_struct:位于8K低端  
            - 进程的内核堆栈：进程处于内核态时使用，位于8K高端  
        * 堆栈相关的寄存器  
            - esp：堆栈指针(stack pointer)  
            - ebp：基址指针(base pointer)  
        * 存放原因  
            这对当前进程的定位有好处：因为内核很容易从esp寄存器的值获得指向当前运行进程的PCB指针，由current宏完成。既节省内存，管理上也比较方便。  

+ 内核堆栈的定义  
C语言用如下的一个union结构来方便的表示这样的一个混合体：  
```C
union task_union
{
    struct task_struct task;
    unsigned long stack[INIT_TASK_SIZE/sizeof(long)];
};
```

1. current宏  
    1. 通常，操作系统的进程控制块策略是：OS有一个指向当前运行的进程控制块的指针，以便系统可以随时访问该控制块。  
    2. Linux的设计是：利用其起始地址一定在堆栈所在的两页空间的低地址页边界的特点，根据堆栈指针推算进程控制块的首地址。  
    3. 进程描述符和内核态堆栈之间的配对关系，内核可以很容易的从esp寄存器的值获得当前在CPU上运行的进程的控制块指针。  
    4. 因为这个内存区是8KB大小，内核必须让esp有13位的有效位，以获得进程控制块的基地址。  
    5. 这个工作由current宏来完成，该算法的伪代码:  

    >current = (struct task_struct(*)STACK_POINTE&0xFFFE000)  

    6. 通过将该值低13位清零，来得到8KB空间的起始地址，该地址就是PCB的地址。  
    7. current宏可以看成当前进程的进程控制指针，在内核中直接使用，这样可以节省一个寄存器。  
    8. 比如current->pid返回在CPU上正在执行的进程的PID。  
        ```C
        #define current get_current()
            static inline struct task_struct * get_current(void)
            {
                struct task_struct *current;
                __asm__("andl %%esp,%0;":"=r"(current):""(~8191UL));
                return current;
            }
        ```
        
+ Linux系统进程堆栈结构  
    Linux系统的进程堆栈结构特点是：  
    * 每个进程都要单独分配一个系统堆栈  
    * 由内核态堆栈和PCB组成  
        - 内核态的进程堆栈  
        - 进程描述符信息(task_struct)  
    * 特点  
        - 8192(2^13)字节，两个页框  
        - 占据连续两个页框，且第一个页框起始地址为2^13的倍数  
+ 内核堆栈的小结  
    1. 与内核堆栈存放在一起的不再是进程控制块，而是thread_info结构。但是由于thread_info结构中有一个指向进程控制块的task指针，所以系统仍然使用current宏查找进程控制块。  
    2. 原因分析：  
        * PCB中最频繁被引用的是thread_info。  
        * 随着版本的变化，进程的PCB中的内容越来越多，所需空间越来越大，使得留给内核堆栈的空间过小，所以将部分进程控制块移出这个空间，只在核心栈保留了访问频繁的thread_info  

+ 进程描述符的分配/回收/访问  
    1. 分配：alloc_task_struct()  
    2. 回收：free_task_struct()  
    3. 访问：get_task_struct()  

+ 进程组织中的关键数据结构  
    1. Linux进程描述符中的进程链表成员  
        * struct list_head run_list 优先级相同的进程组成的进程链表  
            - 当CPU空闲时，内核调度程序寻找一个新的进程在CPU上运行时，必须只考虑可运行进程，因为扫描整个进程链表效率很低  
            - 由此，引入了可运行状态的双向循环链表，也叫运行队列(running queue)  
            - 进程描述符使用list_head用来实现运行队列  
                ```C
                struct list_head{
                    struct list_head *next,*prev;
                };
                ```
                add_to_runqueue 增加一个可运行进程
                del_from_runqueue 删除一个可运行进程 
                int nr_running 可运行队列的长度，可运行进程的个数
        * struct list_head tasks 链接系统中所有进程的链表  
            - 所有的进程用task_struct中的prev_task和next_task组成一个双向循环链表，链表的头和尾都是init_task。  
            - init_task的进程描述符是预先由编译器分配的，在运行的过程中保持不变  
                ```C
                //插入一个进程描述符
                #define SET_LINKS(p)do{\
                    (p)->next_task = &init_task;\
                    (p)->prev_task = init_task.prev_task; \
                    init_task.prev_task->next_task = (p); \
                    init_task.prev_task = (p); \
                    (p)->p_ysptr = NULL; \
                    if(((p)->p_osptr = (p)->p_pptr->p_cptr) != NULL) \
                        (p)->p_osptr->p_ysptr = p; \
                    (p)->p_pptr->p_cptr = p; \
                }while(0)
                //宏扫描整个进程链表  
                #define for_each_task(p) \
                    for(p = &init_task;(p = p->next_task)!= &init_task;)
                ```
        * struct list_head children 链接所有孩子节点进程的链表  

        * struct list_head sibling 链接所有兄弟节点进程的链表  

        * pidhash表  
            - 为了快速定位进程，系统以PID为关健字建立了一个哈希表。若知道进程PID，可以通过该哈希表很快找到该进程。  
            - 如：POSIX规范中规定的系统调用kill()是基于进程号向其它进程发送信号。因此，需要一种机制来高效地完成从进程号到进程描述符地址的转换。  
            ```C
            struct task_struct *pidhash[PIDHASH_SZ];
            #define PIDHASH_SZ(4096 >> 2)
            #define pid_hashfn(x)((((x) >> 8)^(x))&(PIDHASH_SZ - 1))  
            ```
            - 用pid_hashfn宏把PID转换成哈希表的索引  
            - hash_pid(),unhashpid()在pidhash表中分别插入和删除一个进程：  
        * 链表的遍历  
            - 遍历整个链表结构  

                >\#define list_for_each(pos,head)

            - 遍历链表节点特定数据项  

                >\#define list_for_each_entry(pos,head,member)

        * Linux内核链表成员的访问  
            list_entry(ptr,type,member)  
        * 进程的家族关系  
            - 所有进程都是init进程(pid=1)的后代  
            - 每个进程必须有一个父进程  
            - 每个进程可以拥有零个或者多个子进程  
            - 具有相同父进程的进程称为兄弟  
            
            1. 进程家族关系结构定义如下：  
                ```C
                /* 父进程(original parent) */
                struct task_struct *p_opptr;
                /* 父进程(parent) */
                struct task_struct *p_pptr;
                /* 子进程(youngest child) */
                struct task_struct *p_cptr;
                /* 新兄弟进程(younger sibling) */
                struct task_struct *p_ysptr;
                /* 老兄弟进程(older sibling) */
                struct task_struct *p_osptr;
                ```
            2. Linux中的0号进程，是所有进程的祖先，在OS启动时创建。由它执行cpu_idle()函数，当没有其他进程处于TASK_RUNNING的时候，调度进程会选择0号进程运行。  
            3. 0号进程创建1号进程，1号进程通常称为init进程。它首先创建一些后台进程来维护系统，然后进行系统配置，执行shell编写的初始化程序，然后转入用户态运行。  

        * Linux进程家族信息访问  
            1. 获取父进程  

                >struct task_struct *my_parent = current->parent;  

            2. 获取所有子进程(实质是遍历list_head结构)  
                
                >struct task_struct *task;  
                >struct list_head *list;  
                >list_for_each(list, &current->children){  
                >   task = list_entry(list,struct task_struct, sibling);  
                >}  

        * 等待队列  
            1. 当要把除了TASK_RUNNING状态之外的进程组织在一起时，linux使用了等待队列  
            2. 等待队列使得进程可以在事件上进行条件等待，并且当等待的条件为真时，由内核唤醒它们。尤其是对中断处理、进程同步和定时用处很大。  
            3. 等待队列是以双循环链表为基础数据结构。  
                ```C
                struct __wait_queue{
                    unsigned int flags;
                    struct task_struct *task;
                    struct list_head task_list;
                };
                typedef struct __wait_queue wait_queue_t;
                ```
            4. 其元素包括指向进程描述符的指针。每个等待队列都有一个等待队列头(wait_queue_head)，等待队列头是一个类型为wait_queue_head_t的数据结构。  
                
                + 等待队列头  
                    ```C
                    struct __wait_queue_head
                    {
                        spinlock_t lock;
                        struct list_head task_list;
                    };
                    typedef struct __wait_queue_head wait_queue_head_t;
                    ```
                    
                + 等待队列链表  
                + 等待队列  
                + 进程等待  
                + 进程的唤醒  
4. Linux中的进程控制  
5. 线程实现机制  
