---
title: "Gdb Learning"
date: 2020-10-21T21:00:21+08:00
Description: "主要是对《Debugging with GDB》进行的学习"
Tags: ["调试工具"]
Categories: ["Linux"]
DisableComments: false
---

主要是学习《Debugging with GDB》的笔记  

<!--more-->
### 1. gdb简介  
像gdb这样的调试工具的目的是让你进入另一个程序内部了解它是怎么运行的或者当程序crash的时候发生了什么。  
gdb能做四件事来帮助你捕获bugs:  
- 启动程序，可以按照你想要的方式启动  
- 在特定的条件下暂停你的程序  
- 在程序暂停时查看程序当前的状态  
- 改变你程序的状态，尝试修正bug的错误进行到下一步  
### 2. 简单的gdb使用例子
在早期版本的GNU m4程序存在一个bug:有时，当改变默认的标签字符串时，用一个宏来定义另一个宏的命令就无法工作了，如下列的m4工作会话，定义了一个宏foo来表示0000，然后用m4的defn来定义宏bar表示foo。然而当修改开标签为<QUOTE>和闭标签为<UNQUOTE>时，就不能用相同的操作定义一个新的同样的宏baz：  
``` shell
$ cd /gun/m4  
$ ./m4  
define(foo,0000)  

foo  
0000  
define(bar,defn('foo'))  

bar  
0000  
changequote(<QUOTE>,<UNQUOTE>)  

define(baz,defn(<QUOTE>foo<UNQUOTE>))  
baz
```  
接下来可以使用gdb进行调试m4了  
``` shell  
$ gdb m4  
(gdb)  
#设置gdb显示的宽度，包括命令前的“(gdb)”这几个字符的宽度
(gdb) set width 70  
#根据m4的源码找到对应的函数，并设置断点
(gdb) break m4_changequote  
#使用run命令在gdb下启动m4  
(gdb) run  

define(foo,0000)  

foo  
0000  

#调用changequote来触发断点。此时gdb会中断m4的执行，并展示当前的上下文相关信息  
changequote(<QUOTE>,<UNQUOTE>)  

Breakpoint 1,m4_changequote(argc=3,argv=0x33c70) at builtin.c:879  
879     if(bad_argc(TOKEN_DATA(argv[0]),argc,1,3))  

#可以使用n(next)命令来继续执行当前函数的下一行  
(gdb) n  
882     set_quotes((argcc>=2)?TOKEN_DATA_TEXT(argv[1])\
    :nil,    

#set_quotes看起来又像一个函数，可以使用s(step)命令代替next，step可以进入到执行子函数的下一行  
(gdb) s  
set_quotes(lq=0x34c78 "<QUOTE>",rq=0x34c88 "UNQUOTE") at input.c:530  
530     if(lqutoe ! = def_lquote)  

#可以使用backtrace(简写为bt)来展示被挂起的子函数的堆栈信息(包括参数信息)  
(gdb) bt  

#可以使用p(print)命令来查看变量的值  
(gdb) p lquote  
$1 = 0x35d40 "<QUOTE>"  

#可以使用l(list)命令来列出当前行前后10行的内容  
(gdb) l  

#因为变量的值有问题而需要对变量进行修改的时候，同样可以使用p命令  
(gdb) p len_lquote = strlen(lquote)  
(gdb) p len_rquote = strlen(rquote)  

#使用c(continue)命令让程序继续执行  
(gdb) c  

#使用ctrl+d结束程序，并使用q(quit)退出gdb  
(gdb) q
```