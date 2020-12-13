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
在早期版本的GNU m4程序存在一个bug:有时，当改变默认的标签字符串时，用一个宏来定义另一个宏的命令就无法工作了，如下列的m4工作会话，定义了一个宏foo来表示0000，然后用m4的defn来定义宏bar表示foo。然而当修改开标签为```<QUOTE>```和闭标签为```<UNQUOTE>```  时，就不能用相同的操作定义一个新的同样的宏baz：  
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
```  shell
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

### 3. 进入和退出GDB   

+ 输入'gdb'进入GDB  
+ 输入'quit'或 Ctrl-d退出  

#### 启动GDB  
通过使用程序gdb来调用GDB。一旦启动，GDB会从终端读取命令直到你退出GDB。  
``` shell     
#最常用的启动GDB的方法是带上一个可执行的程序作为参数  
gdb program  

#可以使用可执行程序和特定的core文件来启动GDB  
gdb program core  

#如果你想调式一个运行中的进程，可以使用特定的进程ID作为第二个参数或者使用选项-p  
gdb program 1234   
gdb -p 1234  
```
你能使用--args选项让gdb传递可执行文件后面的参数给次级程序  
``` shell   
#这会使用gdb来调试gcc程序，并将'-01 -c foo.c'作为gcc的参数  
gdb --args gcc -02 -c foo.c   

#让GDB启动时不显示开头部分的信息
gdb --silent  

#使用help显示所有可用选项及其简述  
gdb -help
```

+ 选择文件  
当GDB启动时，会读取选项以外的参数作为指定的可执行文件和core文件(或进程ID)。和分别使用'-se'和'-c'(或'-p')选项指定的参数想同。(GDB会读取第一个和选项标识无关的参数作为可执行文件，同'-se'后面携带的参数一致。同理会读取第二个和选项标识无关的参数作为core文件，和'-c'/'-p'选项后面鞋带的参数一致)。如果第二个参数是数字的形式，GDB首先会尝试将其作为进程号解释，如果失败，尝试做为一个core文件进行解释。如果你的core文件是以数字命名的，你可以是用前缀'./'来防止GDB将其作为进程号来对待。  
当GDB没有配置core文件的支持，像一些嵌入式设备，GDB会忽略第二个参数。  
许多选项有长和短的形式，如果长的形式你能截取到能够确定的长度，GDB同样可以识别。  
>-symbols file  
>-s file  **从文件中加载syabols table**
>
>-exec file  
>-e file  **用文件file作为可执行文件，并且结合core dump检查数据**
>
>-se file  **从文件中加载symbol table并且用其作为可执行文件**
>
>-core file  
>-c file  **使用文件作为一个core dump进行检查**
>
>-pid number  
>-p number  **连接到进程号为number的进程，同attach命令一样**
>
>-command file  
>-x file  **从文件中执行命令，文件中的内容就是确切的命令**
>  
>-eval-command command  
>-ex command  **执行一条单独的GDB命令，这个选项能被使用多次来执行多条命令，并且可以和-x相结合**  
>
>-init-command file  
>-ix file  **在inferior之前和gdbinit之后，执行文件中的命令**  
>
>-init-eval-command command  
>-iex command  **在inferior之前和gdbinit之后执行一条单独的GDB命令**
>
>-directory directory  
>-d directory  **添加搜索源和脚本文件的路径**
>
>-r  
>-readnow **立刻加载所有的symbole file的内容，而不是按需加载。可能在启动的时候较慢，但是操作的是后更快**  
>-readnever  
+ 选择模式  
你能以各种各样的模式来启动GDB，比如batch模式或quiet模式  
>-nx  
>-n **不执行初始化文件中的命令，有三个初始化文件按下列顺序加载：**  
>
>   + system.gdbinit  
>       这是系统范围的初始化文件。其位置由--with-system-gdbinit配置选项指定。是GDB启动时最先加载的，在命令行选项被处理之前。
>   + system.gdbinit.d  
>       这是系统范围的初始化目录。其位置由--with-system-gdbinit-dir配置选项指定，在GDB启动并加载了system.gdbinit配置后会将次目录中的文件按字母序进行加载，同样在命令行选项被处理之前。以.py/.scm或.gdb结尾的文件会被解释为GDB命令。GDB不会递归处理此目录的子目录。
>   + ~/.gdbinit  
>       这是在home目录下的初始化文件。在system.gdbinit之后，命令行选项处理之前进行加载。
>   + ./.gdbinit  
>       这是当前目录下的初始化文件。会在最后加载，在命令行选项处理之后(除了-x和-ex这两个选项以外,这两个选项会在最后处理)。
>   
>-nh  **不执行~/.gdbinit初始化文件中的命令。**
>
>-quiet  
>-silent  
>-q  **不打印介绍和版本信息，这些信息在batch模式下同样不会显示**
>  
>-batch **使用batch模式，在执行完'-x'命令指定的命令文件后以0状态退出。如果执行命令文件中的GDB命令出错时以非零的状态退出。该模式不支持分页，可以无限制的设置终端的宽高。**
>
>-batch-silent **使用silently的batch模式，GDB所有的stdout输出都不会显示(stderr除外)。这个模式比'-silent'更加quiet，但是在交互式会话中没用。**
>
>-return-child-result **返回码会是子进程(被调试进程)的返回码，(GDB非正常退出，用户带值退出，子进程没有运行或不允许终止时返回-1。这些情况除外)**
>
>-nowindows  
>-nw **如果GDB有一个图形化的界面，此选项表示GDB只使用命令行模式**
>
>-windows
>-w **如果GDB有GUI，那么次选项要求GDB尽量使用图形界面**
>
>-cd directory **使用directory作为工作目录，取代当前的目录**
>
>-data-directory directory
>-D directory **使用directory作为其数据目录**
+ 在启动其间GDB做了什么  
GDB在启动期间做的事情包括如下几点:  

1. 加载命令行指定的命令解释器。  
2. 加载system-wide的初始化文件和在system-wide gdbinit directory目录中的所有文件(以.gdb结尾的GDB命令文件或其他支持的教本文件)。  
3. 加载HOME目录下的初时化文件，并执行其中的命令。  
4. 加载'-iex'和'-ix'选项设置的命令及命令文件。  
5. 处理命令行选项和操作对象。  
6. 加载执行当前工作目录中的初始化文件的命令，只有当前的工作目录和你的home目录不同时才生效。因此你可能有多个初始化文件，一个在home目录中，另外的可能在你启动GDB调试程序的目录下。  
7. 在命令行指定了一个执行程序、进程或core file后，GDB会加载程序及其库提供的一些自动加载的脚本文件，如果你想在启动其间关闭自动加载可以使用以下命令:  
```$gdb -iex "set auto-load python-scripts off" program```  
8. 加载'-ex'和'-x'选项设置的命令及命令文件。  
9. 加载history file中的历史记录命令。  

初始化文件使用和命令文件相同的语法，并且GDB的处理方式也是相同的。在home目录中的初始化文件能设置参数来影响子进程的命令行选项和操作对象。当使用'-nx'选项时，初始化文件不会被执行。  
可以使用```gdb --help```来查看gdb在启动时家在了那些初始化文件。  

#### 退出GDB  
>```quit [expressiong] q```  

用quit命令或者键入文件结束符(ctrl-d)退出GDB。如果你没有应用expression，GDB会按普通方式结束；否则会以expression的结果作为错误码退出。  
一个中断(ctrl-c)不会退出GDB，而是终止当前进程的GDB命令并返回到GDB命名视图。在任何时候使用中断命令都是安全的，因为在非安全的时段GDB是不会让其产生作用的。  
如果你用GDB来attach了一个进程或者设备，你能使用detach解除attach。  

#### Shell命令  
如果在GDB调试其间想要执行相关的shell命令，不需要退出GDB，你可以使用GDB的shell命令。  
>```
>shell command-string
>!command-string
>```  
 
调用一个标准的shell来执行command-string命令，在！和command-string之间是没有空格的。如果在操作系统中设置了SHELL这个环境变量，则会使用该变量设置的shell来执行command-string，否则是用默认的(/bin/sh)来执行command-string。  

make工具在开发环境中经常被使用，你不一定要用shell命令来调用make命令，可以直接使用make命令。  
>```make make-args```  

执行带参数的make程序。等价于```shell make make-args``` 

>```  shell
>pipe [command]|shell_command  
>| [command]|shell_command  
>piep -d delim command delim shell_command  
>| -d delim command delim shell_command  
>```  

执行command命令并将其输出发送给shell_command。如果没有command，上一次执行的command会重复执行。  
当command中包含'|'界定字符时，选项'-d delim'能指定一个备用界定字符串delim用以区分command和shell_command。  
>```  shell
>(gdb)p var  
>$1={
>    black = 144,
>    red = 233,
>    green = 377,
>    blue = 610,
>    white = 987
>}  
>(gdb) pipe p var|wc  
>    7 19 80  
>(gdb)|p var|wc -l  
>    7  
>#使用!字符作为备用分隔符，因为command中包含|字符。
>(gdb)| -d ! echo this contains a | char\n ! sed -e 's/|/PIPE/' this contains a PIPE char  
>#使用xxx字符串作为备用分隔符。
>(gdb)| -d xxx echo this contains a | char! \n xxx sed -e 's/|PIPE/' this contains a PIPE char  
>```  

#### 日志输出  
你可能想要保存GDB命令行的输出到一个文件中。下列有好几条命令来控制GDB的日志。  
>```  shell 
>#打开日志功能  
>set logging on  
>#关闭日志功能  
>set logging off  
>#修改当前日志文件的名称，默认的日志文件名称是gdb.txt  
>set logging file file  
>#GDB默认是在日志文件中append，如果你想覆盖原有的日志文件则需要设置overwrite  
>set logging overwrite [on|off]  
>#GDB默认是在终端和日志文件中都输出，如果你想只在日志文件中输出需要设置redirect  
>set logging redirect [on|off]  
>#GDB调试的输出同样会在终端和日志文件中都输出，如果你想只在日志文件中输出需要设置debugredirect 
>```  

### 4. GDB命令  
对于明确的命令你可以使用缩写的GDB命令。通过输入**RETURN**，你能重复上一条命令，同时你能使用**TAB**按键来进行命令的自动补全或进行提示。  
#### 命令语法  
一个GDB命令是单行的输入。不会限制命令有多长。通常以一个命令名开始，后面可以跟随一些参数，这些参数取决于用的什么命令。比如，命令step可以带上一个参数表示step进行的次数'step 5'。你也可以用不带参数的step命令，一些命令是可以不用带参数的。  
一些GDB的命令名可以缩减当简写不会模棱两可的时候。文档中的一些其他的命令简写可能是针对其他命令的。在某些情况下，模棱两可的缩写也是被允许的；比如's'通常等价于'step'命令，尽管有一些是用's'表示'start'。你能在'help'命令后面带上缩写作为参数来查看缩写是什么含义。  
输入一个空行(键入**RETURN**)可以重复上一条命令。一些命令(如，run)不适用这一特性，因为这些命令重复可能会造成一些问题并且你也不希望重复运行这些命令。  
当你使用**RETURN**来重复list和x命令时，会使用新的参数而不是输入的确定的参数，这样容易扫描源码和内存。  
GDB使用**RETURN**的另一个作用是显示过量的输出，和shell命令more相似。  
GDB的注释使用#号符。  
ctrl-o绑定在重复一个复杂的命令序列时非常有用。  
#### 命令设置  
#### 命令补全  
#### 命令选项  
#### 命令别名  
#### 获取帮助
