---
title: "Compiler_Link_Load Learning"
date: 2022-08-30T15:46:38+08:00
Description: ""
Tags: ["编译"]
Categories: ["C"]
DisableComments: false
---

主要为程序员的自我修养--编译、链接、装载的相关学习
<!--more-->>

### 1. 温故而知新
1. 从Hello World说起  
    ``` C
    #include <stdio.h>

    int main()
    {
        printf("Hello World\n");
        return 0;
    }
    ```

    对于一个Hello World需要了解下述问题:
    * 程序为什么要被编译器编译了之后才可以运行?  
    * 编译器在把C语言程序转换成可以执行的机器码的过程中做了什么，怎么做的?  
    * 最后编译出来的可执行文件里面是什么？除了机器码还有什么？它们怎么存放的，怎么组织的?  
    * \#include<stdio.h>是什么意思？把stdio.h包含进来意味着什么？C语言库又是什么？它怎么实现的？  
    * 不同的编译器(Microsoft VC、GCC)和不同的硬件平台(x86、SPARC、MIPS、ARM)，以及不同的操作系统(Windows、Linux、UNIX),最终编译出来的结果一样吗？为什么？  
    * Hello World程序是怎么运行起来的？操作系统怎么装载它的？它从哪儿开始执行，到哪儿结束？main函数之前发生了什么？main函数结束以后又发生了什么？  
    * 如果没有操作系统，Hello World可以运行吗？如果要在一台没有操作系统的机器上运行Hello World需要什么？应该怎么实现？  
    * printf是怎么实现的？它为什么可以有不定数量的参数？为什么它能够在终端上输出字符串?  
    * Hello World程序在运行时，它在内存中是什么样子的？  

2. 万变不离其宗  
3. 站的高，望的远  
    系统软件可以分成两块，一块是平台性的，比如操作系统内核、驱动程序、运行库和系统工具；另一块是用于程序开发的，比如编译器、汇编器、链接器等开发工具和开发库。  
    系统软件体系结构中，各种软件的位置如下:  
    ```
    ===================================
    |Application:  |Development Tools:|
    |Web Browser   | c/c++ compiler   |
    |Video Player  |  Assembler       |
    |Word Processor|  Library Tools   |
    |...           |  ...             |
    ===================================
    ----- Operation System Api --------
    ===================================
    |        Runtime Library          |
    ===================================
    --------- System Call -------------
    ===================================
    |    Operating System Kernel      |
    ===================================
    ------ Hardware Specification -----
    ===================================
    |             Hardware            |
    ===================================
    ```
    每个此层次之间都需要相互通信，既然需要通信就必须有一个通信的协议，我们一般将其称为接口(Interface)，接口的下面那层是接口的提供者，由它定义接口;接口的上面那层是接口的使用者，它使用该接口来实现所需要的功能。除了硬件和应用程序，其他都是所谓的中间层，每个中间层都是对它下面的那层的包装和扩展。  
    软件体系中，位于最上层的是应用程序，开发工具与应用程序是属于同一个层次的，因为它们都使用一个接口，那就是操作系统应用程序编程接口(Application Programming Interface)。应用程序接口的提供者是运行库。比如Linux下的Glibc库提供POSIX的API;Windows的运行库提供Windows API，最常见的32位Windows提供的API又称为Win32。  
    运行库使用操作系统提供的系统调用接口(System call Interface)，系统调用接口在实现中往往以软中断(Software Interrupt)的方式提供，比如Linux使用0x80号中断作为系统调用接口，Windows使用0x2E号中断作为系统调用接口。  
    操作系统内核层对于硬件层来说是硬件接口的使用者，而硬件是接口的定义者，硬件的接口定义决定了操作系统内核，具体来讲就是驱动程序如何操作硬件，如何与硬件进行通信。这种接口往往被叫做硬件规格(Hardware Specification)，硬件的生产厂商负责提供硬件规格，操作系统和驱动程序的开发者通过阅读硬件规格文档所规定的各种硬件编程接口标准来编写操作系统和驱动程序。  

4. 操作系统做什么  
    操作系统的一个功能是提供抽象的接口，另外一个主要功能是管理硬件资源。  
    计算机中的资源主要分CPU、存储器(内存和磁盘)和I/O设备。  
    * 不要让CPU打盹  
        多道程序(Multiprogramming)： 通过一个监控程序，当某个程序暂时无需使用CPU时，监控程序就把另外的正在等待CPU资源的程序启动，使得CPU能够充分地利用起来。  
        分时系统(Time-Sharing System): 每个程序运行一段时间以后都主动让出CPU给其他程序，使得一段时间内每个程序都有机会运行一小段时间。  
        多任务系统(Multi-tasking)：操作系统接管了所有的硬件资源，并且本身运行在一个受硬件保护的级别。所有的应用程序都以进程(Process)的方式运行在比操作系统权限更低的级别，每个进程都有自己独立的地址空间，使得进程之间的地址空间相互隔离。CPU由操作系统统一进行分配，每个进程根据进程优先级的高低都有机会得到CPU,但是，如果运行超出了一定的时间，操作系统会暂停该进程，将CPU资源分配给其他等待运行的进程。如果操作系统分配给每个进程的时间都很短，即CPU在多个进程间快速地切换，从而造成了很多进程都在同时运行的假想。  
        
    * 设备驱动  
        操作系统作为硬件层的上层，它是对硬件的管理和抽象。对于操作系统上面的运行库和应用程序来说，它们希望看到的是一个统一的硬件访问模式。  
        当成熟的操作系统出现以后，硬件逐渐被抽象成了一系列概念。在UNIX中，硬件设备的访问形式跟访问普通的文件形式一样;在Windows系统中，图形硬件被抽象成了GDI,声音和多媒体设备被抽象成了DirectX对象，磁盘被抽象成了普通文件系统，等等。这些繁琐的硬件细节全都交给了操作系统，具体地讲是操作系统中的硬件驱动程序来完成。驱动程序可以看作是操作系统的一部分，它往往跟操作系统内核一起运行在特权级，但它又与操作系统内核之间有一定的独立性，使得驱动程序有比较好的灵活性。接下来以一个读取文件为例子来看看操作系统和驱动程序在这个过程中扮演了什么样的角色。  
        在Linux操作系统中，要读取这个文件的前4096个字节时，我们会使用一个read的系统调用来实现。文件系统收到read请求之后，判断出文件的前4096个字节位于磁盘的1000号逻辑扇区到1007号逻辑扇区。然后文件系统就向硬盘驱动发出一个读取逻辑扇区为1000号开始的8个扇区的请求，磁盘驱动程序收到这个请求以后就向硬盘发出硬件命令。向硬件发送I/O命令的方式有很多种，其中最为常见的一种就是通过读写I/O端口寄存器来实现。  

5. 内存不够怎么办  
    操作系统的多任务功能使得CPU能够在多个进程之间很好地共享，从进程的角度看好像是它独占了CPU而不用考虑与其他进程分享CPU。操作系统的I/O抽象模型也很好地实现了I/O设备的共享和抽象，那么唯一剩下的就是内存分配问题。 
    在早期的计算机中，程序是直接运行在物理内存上的，也就是说，程序在运行时所访问的地址都是物理地址。如何将计算机上有限的物理内存分配给多个程序使用。128MB的内存，程序A运行需要10MB,程序B需要100MB,程序C需要20MB。如果需要同时运行程序A和B，直接的做法是将内存前10MB分配给程序A,10MB-11MB分配给B。这样A和B能同时运行，但是这种简单的内存分配问题很多。   
    
    >* 地址空间不隔离  
    >   所有程序都直接访问物理地址，程序所使用的内存空间不是相互隔离的。恶意的程序可以很容易改写其他程序的内存数据，以达到破坏的目的。有些非恶意的，有bug的程序可能不小心修改了其他程序的数据，就会使其他程序也崩溃。  
    >* 内存使用效率低   
    >   由于没有有效的内存管理机制，通常需要一个程序执行时，监控程序就将整个程序装入内存中然后开始执行。如果我们忽然需要运行程序C,那么这时内存空间其实已经不够了，这时我们可以用一个办法是将其他程序的数据暂时写到磁盘里面，等到需要用到的时候再读回来。整个过程中有大量的数据在换入换出，导致效率十分低下。  
    >* 程序运行的地址不确定  
    >   因为程序每次需要装入运行时，我们都需要给它从内存中分配一块足够大的空闲区域，这个空闲区域的位置是不确定的。  
    
    解决这几个问题的思路就是使用我们前文提到过的增加中间层，即使用一种间接的地址访问方法。把程序给出的地址看作是一种虚拟地址(Virtual Address)，然后通过某些映射的方法，将这个虚拟地址转换成实际的物理地址。这样，只要能够妥善地控制这个虚拟地址到物理地址的映射过程，就可以保证任意一个程序所能够访问的物理内存区域跟另外一个程序相互不重叠，以达到地址空间隔离的效果。  

    + 关于隔离  
        所谓的地址空间是个比较抽象的概念，你可以把它想象成一个很大的数组，每个数组的元素是一个字节，而这个数组大小由地址空间的地址长度决定，比如32位的地址空间的大小为2^32个字节，即4GB，地址空间的有效地址为0x00000000-0xffffffff。地址空间分两种:虚拟地址空间和物理地址空间。物理地址空间是实实在在的，存在于计算机中，而且对于每一台计算机来说只有唯一的一个，你可以把物理空间想象成物理内存。虚拟地址空间是指虚拟的、人们想象出来的地址空间，其实它并不存在，每个进程都有自己独立的虚拟空间，而且每个进程只能访问自己的地址空间，这样就有效地做到了进程的隔离。  

    + 分段(Segmentation)  
        基本思路是把一段与程序所需要的内存空间大小的虚拟空间映射到某个地址空间。这个映射过程由软件来设置，比如操作系统来设置这个映射函数，实际的地址转换由硬件完成。这样解决了地址空间不隔离和地址不确定的问题，因为当访问的地址超过虚拟空间的地址时，硬件就会判断这是一个非法的访问，拒绝这个地址请求。同时对于每个程序来说，分配到物理地址的那一个区域都是透明的，它们不关心物理地址的变化，只需要按照虚拟地址空间来编写程序等。   
        但是还会存在内存使用效率的问题，分段对内存区域的映射还是按照程序为单位，如果内存不足，被换入换出到磁盘的都是整个程序。实际上，根据程序的局部性原理，当一个程序在运行时，在某个时间段内，它只是频繁地用到了一小部分数据，也就是说，程序的很多数据其实在一个时间段内都是不会被用到的。这样就出现了更小粒度的内存分割和映射方法，即分页。  

    + 分页(Paging)  
        基本方法是把地址空间人为地等分成固定大小的页，每一页的大小由硬件决定，或硬件支持多种大小的页，由操作系统选择决定页的大小。比如Intel Pentium系列处理器支持4KB或4MB的页大小，那么操作系统可以选择每页大小为4KB，也可以选择每页大小为4MB，但是在同一时刻只能选择一种大小，所以对整个系统来说，页就是固定大小的。使用32位的pc机，虚拟地址为4GB，按4KB每页分的话，总共有1048576个页。物理空间也是同样的分法。  
        虚拟空间的页叫虚拟页(VP,Virtual Page)，物理内存中的页叫做物理页(PP,Physical Page),磁盘中的页叫做磁盘页(DP,Disk Page)。虚拟空间的有些页被映射到同一个物理页，这样就可以实现内存共享。  
        当虚拟页不在内存中，但是进程需要用到的时候，硬件会捕获到这个消息，就是所谓的页错误(Page Fault)，然后操作系统接管进程，负责将虚拟页从磁盘中读出来并且装入内存，然后将内存中的这两个页与虚拟页建立映射关系。  
        保护也是页映射的目的之一，简单地说就是每个页可以设置权限属性，谁可以修改，谁可以访问等，而只有操作系统有权限修改这些属性，那么操作系统就可以做到保护自己和保护进程。  
        虚拟存储的实现需要依靠硬件的支持，对于不同的CPU来说是不同的。但是都采用一个叫做MMU(Memory Management Unit)的部件来进行页映射。CPU发出Virtual Address，经过MMU转换以后变成Physical Address。一般MMU都集成在CPU内部了，不会以独立的部件存在。  

6. 众人拾柴火焰高  
    * 线程基础  
        - 什么是线程  
            线程(Thread)，有时被称为轻量级进程(Lightweight Process,LWP)，是程序执行流的最小单元。一个标准的线程由线程ID、当前指令指针(PC)、寄存器集合和堆栈组成。通常意义上，一个进程由一个到多个线程组成，各个线程之间共享程序的内存空间(包括代码段、数据段、堆等)以及一些进程级的资源(如打开文件和信号)。一个经典的线程与进程的关系如下图:  
            
            >|=============================|
            >|--code--data--process--file--|
            >||=======| |=======| |=======||
            >|| reg   | | reg   | | reg   ||
            >|| stack | | stack | | stack ||
            >||thread1| |thread2| |thread3||
            >||=======| |=======| |=======||
            >|=============================|
            
            使用多线程的原因有以下几点:  

            1. 某个操作可能会陷入长时间等待，等待的线程会进入睡眠状态，无法继续执行。多线程执行可以有效利用等待的时间。典型的例子是等待网络响应。  
            2. 某个操作(常常是计算)会消耗大量的时间，如果只有一个线程，程序和用户之间的交互会中断。多线程可以让一个线程负责交互，另一个线程负责计算。  
            3. 程序逻辑本身要求并发操作，例如一个多端下再软件(如Bittorrent)。  
            4. 多CPU或多核计算机，本身具备同时执行多个线程的能力，因此单线程无法全面地发挥计算机的全部计算能力。  
            5. 相对于多进程应用，多线程在数据共享方面效率要高很多。  

        - 线程的访问权限  
            线程的访问非常自由，它可以访问进程内存里的所有数据，甚至包括其他线程的堆栈(如果它知道其他线程的堆栈地址，这是很少见的情况)，但实际运用中线程也拥有自己的私有存储空间：  

            1. 栈(尽管并非完全无法被其他线程访问，但一般情况下仍然可以认为是私有的数据)。  
            2. 线程局部存储(Thread Local Storage,TLS)。线程局部存储是某些操作系统为线程单独提供的私有空间，但通常只具有很有限的容量。  
            3. 寄存器(包括PC寄存器)，寄存器是执行流的基本数据，因此为线程私有。  

        - 线程调度与优先级  
            线程总是"并发"执行的，当线程数量小于等于处理器数量时，线程的并发是真正的并发，不同的线程运行在不同的处理器上，彼此之间互不相干。但对于线程数量大于处理器数量的情况，线程的并发会受到一些阻碍，因为此时至少由一个处理器会运行多个线程。  
            在单处理器对应多线程的情况下，并发是一种模拟出来的状态。操作系统会让这些多线程程序轮流执行，每次仅执行一小段时间(通常是几十到几百毫秒)，这样的一个不断在处理器上切换不同的线程的行为称之为线程调度(Thread Schedule)。在线程调度中，线程通常拥有至少三种状态，分别是:  

            1. 运行(Running)：此时线程正在执行。  
            2. 就绪(Ready)：此时线程可以立刻运行，但CPU已经被占用。  
            3. 等待(Waiting)：此时线程正在等待某一事件(通常是I/O或同步)发生，无法执行。  
            
            处于运行中线程拥有一段可以用于执行的时间，这段时间称为时间片(Time Slice)，当时间片用尽的时候，该线程将进入就绪状态。如果在时间片用尽之前线程就开始等待某事件，那么它将进入等待状态。每当一个线程离开运行状态时，调度系统就会选择一个其他的就绪线程继续执行。在一个处于等待状态的线程所等待的事件发生之后，该线程将进入就绪状态。  
            线程调度自多任务操作系统问世以来就不断地被提出不同的方案和算法。现在主流的调度方式尽管各不相同，但都带有优先级调度(Priority Schedule)和轮转法(Round Robin)的痕迹。  
            轮转法是让各个线程轮流执行一小段时间的方法。这决定了线程之间交错执行的特点。而优先级调度则决定了线程按照什么顺序轮流执行。在具有优先级调度的系统中，线程都拥有各自的线程优先级(Thread Priority)。具有高优先级的线程会更早地执行，而低优先级的线程常常要等待到系统中已经没有高优先级的可执行的线程存在时才会能够执行。  
            频繁等待的线程称之为IO密集型线程(IO Bound Thread)，而把很少等待的线程称为CPU密集型线程(CPU Bound Thread)。IO密集型线程总是比CPU密集型线程容易得到优先级提升。  
            在优先级调度下，存在一种饿死(Starvation)的现象，一个线程被饿死，是说它的优先级较低，在它执行之前，总是有较高优先级的线程试图执行，因此这个低优先级线程始终无法执行。当一个CPU密集型的线程获得较高优先级时，许多低优先级的线程就很可能饿死。而一个高优先级的IO密集型线程由于大部分时间都处于等待状态，因此相对不容易造成其他线程饿死。为了避免饿死现象，调度系统常常会逐步提升那些等待了过长时间的得不到执行的线程的优先级。这样，一个线程只要等待足够长的时间，其优先级一定会提高到足够让它执行的程度。  

        - 可抢占线程和不可抢占线程  
            
        - Linux的多线程  
    * 线程安全  
        - 竞争与原子操作  
        - 同步与锁  
        - 可重入(Reentrant)与线程安全  
        - 过度优化  
    * 多线程内部情况  
        - 三种线程模型  

### 2. 编译和链接
### 3. 目标文件里有什么
### 4. 静态链接
### 5. Windows PE/COFF
### 6. 可执行文件的装载与进程
### 7. 动态链接
### 8. Linux共享库的组织
### 9. Windows的动态链接
### 10. 内存
### 11. 运行库
### 12. 系统调用与API
### 13. 运行库实现  