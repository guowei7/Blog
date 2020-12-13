---
title: "C Learning Two"
date: 2020-12-12T19:05:35+08:00
Description: "C primer Plus继续学习"
Tags: ["C"]
Categories: ["编程语言"]
DisableComments: false
---

由于C-Learning.md存在损坏，故重开一篇继续记录  
<!--more-->
### 7. C控制语句：分支和跳转  
本章介绍以下内容：  
关键字：if、else、switch、continue、break、case、default、goto  
运算符：&&、||、？ ：   
函数：getchar()、putchar()、ctype.h系列  
如何使用if和if else语句，如何嵌套它们   
在更复杂的测试表达式中用逻辑运算符组合关系表达式  
C的条件运算符  
switch语句  
break、continue和goto语句    
使用C的字符I/O函数：getchar()和puchar()  
ctype.h头文件提供的字符分析函数系列  
#### if语句  
下面的程序是读取一系列数据，每个数据都表示每日的最低问题，然后打印统计的总天数和最低温度在0°以下的天数占总天数的百分比。程序中的循环通过scanf()读入温度值。while循环每迭代一次，就递增计数器增加天数，其中的if语句负责判断0°以下的温度并单独统计相应的天数。  
[colddays.c](../../c-example/colddays.c)  
为避免整数除法，该程序示例把计算后的百分比强制转换为float类型。其实，也不必使用强制类型转换，因为在表达式100.0  * cold_dyas / all_days中，将首先对表达式100.0 * cold_days求值，由于C的自动转换类型规则，乘积会被强制转换成浮点数。但是，使用强制类型转换可以明确表达式转换类型的意图，保护程序免受不同版本编译器的影响。if语句被称为分支语句(branching statement)或选择语句(selection statement)，因为它相当于一个交叉点，程序要在两条分支中选择一条执行。if语句的通用形式如下：  

>if(expression)  
>statement  

如果对expression求值为真(非0),则执行statement：否则，跳过statement。与while循环一样，statement可以是一条简单语句或复合语句。if语句的结构和while语句很相似，它们的主要区别是：如果满足条件执行的话，if语句 只能测试和执行一次，而while语句可以测试和执行多次。  
通常，expression是关系表达式，即比较两个量的大小(如，表达式x > y或c == 6)。如果expression为真(即x大于y，或c == 6),则执行statement。否则，忽略statement。概括地说，可以使用任意表达式，表达式的值为0则为假。  
statement部分可以是一条简单语句，如本例所示，或者是一条用花括号括起来的复合语句(或块)  
注意，即使if语句由复合语句构成，整个if语句仍被视为一条语句。  
#### if else语句   
简单形式的if语句可以让程序选择执行一条语句，或者跳过这条语句。C还提供了if else形式，可以在两条语句之间作选择。  
注意，if else语句的通用形式是：  

>if(expression)
>statement1
>else
>statement2

如果expression为真(非0),则执行statement1；如果expression为假或0，则执行else后面的statement2。statement1和statement2可以是一条简单语句或复合语句。C并不要求一定要缩进，但这是标准风格。缩进让根据测试条件的求值结果来判断执行哪部分语句一目了然。  
如果要在if和else之间执行多条语句，必须用花括号把这些语句括起来成为一个块。   

+ 介绍getchar()和putchar()  
    getchar()函数不带任何参数，它从输入队列中返回下一个字符。例如，下面的语句读取下一个字符输入，并把该字符的值赋给变量ch：  
    ``` C
    ch = getchar();
    ```
    该语句与下面的语句效果相同：  
    ``` C
    scanf("%c",&ch);
    ```
    putchar函数打印它的参数。例如，下面的语句把之前赋给ch的值作为字符打印出来：  
    ``` C
    putchar(ch);
    ```
    该语句与下面的语句效果相同：  
    ``` C
    printf("%c",ch);
    ```
    由于这些函数只处理字符，所以它们比更通用的scanf()和printf()函数更快、更简洁。而且，注意getchar()和putchar()不需要转换说明，因为它们只处理字符。这两个函数通常定义在stdio.h头文件中(而且，他们通常是预处理宏，而不是真正的函数，第16章会讨论类似函数的宏)。