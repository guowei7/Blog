---
title: "Cube Learning"
date: 2022-02-05T11:03:04+08:00
Description: "魔方学习"
Tags: ["Cube"]
Categories: ["Entertainment"]
DisableComments: false
---

<!--记录三阶魔方复原方法-->
### CFOP  
#### 介绍CFOP
三阶魔方公式字母介绍:  

>|符号|含义|符号|含义|符号|含义|
>|:-:|:-:|:-:|:-:|:-:|:-:|
>|U|上层顺时针90度|R|右面顺时针90度|F|前面顺时针90度|
>|U'|上层逆时针90度|R'|右面逆时针90度|F'|前面逆时针90度|
>|U2|上层顺时针180度|R2|右面顺时针180度|F2|前面顺时针180度|
>|D|底层顺时针90度|L|左面顺时针90度|B|后面顺时针90度|
>|D'|底层逆时针90度|L'|左面逆时针90度|B'|后面逆时针90度|
>|D2|底层顺时针180度|L2|左面顺时针180度|B2|后面顺时针180度|
>|E'|中间层俯视顺时针90度|M'|中层向上90度|S|中层向F顺时针90度|
>|E|中间层俯视逆时针90度和D一致|M|中层向下90度,和L方向一致|S'|中层向F逆时针90度|
>|E2|中间层(U)180度|M2|中间层(L)180度|S2|中间层(F)180度|
>|x|整个魔方R方向90度|y|整个魔方U方向90度|z|整个魔方F方向90度|
>|x'|整个魔方R'方向90度|y'|整个魔方U'方向90度|z'|整个魔方F'方向90度|
>|x2|整个魔方R方向180度|y2|整个魔方U方向180度|z2|整个魔方F方向180度|
 
 ***蓝色字体表示左食指拨动，红色字体表示右拇指拨动，橙色字体表示无名指拨动***  
1. CFOP复原的步骤  
    + Cross,复原十字  
    + F2L,复原顶两层的棱块，需要复原四组(红绿白、红蓝白、橙绿白、橙蓝白)  
    + OLL,复原顶面黄色  
    + PLL,复原边上棱角  
    CFOP的公式量有119个但是其复原使用的公式为7个，更加快速。  

2. 转动描述与基础手法  
3. Cross  
    白底在下(顺时针红蓝橙绿,红对橙，蓝对绿)  
    + 一个棱块错误  
        * 错误块白色在第二层  
        * 错误块白色在顶层  
        * 错误块白色在第一层第三层的侧边
    + 两个棱块错误  
    + 三个棱块错误  
    + 四个棱块错误   
4. F2L  
    * 找到第一层和第二层含有同样颜色的棱块和角块的位置  
    * 利用顶层以及空槽，通过一定的旋转方式使用藏棱藏角的方法把相应的棱角组合变成四种标准态的一种。空槽是指做完十字之后，只要有一组棱块和角块的组合未完成，那么那一组区域就可以被认为是空槽。藏棱藏角是用来让棱块或角块旋转到合适位置，一般有四种藏法，角块藏左边，角块藏右边，棱块藏左边，棱块藏右边    
    * 把棱角组合转到正确的位置，做标准态的公式  
    标准态的公式包含如下几个大类:  
    * 角块在第一层，白底在下方，即角块是正确的位置  
        - 棱块在第二层，颜色相反错误  
            公式1:(RU'R'U)y'(R'U2RU'U')(R'UR)  
            逆公式:(R'U'R)(UUR'U'2R)y(U'RUR')  
        - 棱块在第三层正面  
            公式2:(URU'R'U')y'(R'UR)
            逆公式:(R'U'R)y(URUR'U')
        - 棱块在第三层右面  
            公式3:(R'F'RU)(RU'R'F)  
            逆公式:(F'RUR')(U'R'FR)  
    * 角块在第一层，右边色(红)在下方  
        - 棱块在第二层，颜色正确  
            公式4:(RUR'U')(RU2R'U')(RUR')  
            逆公式:(RU'R')(URU'2R')(URU'R')  
        - 棱块在第二层，颜色相反错误  
            公式5:(RF)U(RU'R'F')(U'R')  
            逆公式:(RU)(FRUR')U'(F'R')  
        - 棱块在第三层正面  
            公式6:y'(R'U'RU)(R'U'R)  
            逆公式:(R'UR)(U'R'UR)y  
        - 棱块在第三层右面  
    * 角块在第一层，正面色(蓝)在下方  
        - 棱块在第二层，颜色正确  
            公式8:(RU'R'U)(RU'U'R'U)(RU'R')  
            逆公式:(RUR')(U'RUUR')(U'RUR')  
        - 棱块在第二层，颜色相反错误  
            公式9:(RU)F(RUR'U')F'R'  
            逆公式:RF(URU'R')F'(U'R')
        - 棱块在第三层正面  
            公式10:y'(R'URU')(R'UR)  
            逆公式:(R'U'R)(UR'U'R)y  
        - 棱块在第三层右面 
    * 角块在第三层，白底在上方   
        - 棱块在第二层，颜色正确  
        - 棱块在第二层，颜色错误  
            公式13:(RU'R'U)y'(R'UR)  
            逆公式:(R'U'R)y(U'RUR')  
        - 棱块在第三层，右侧红色在上方6点方向  
        - 棱块在第三层，右侧红色在上方9点方向  
            公式15:y'U'(R'U2)(RU'R'UR)  
            逆公式:(R'U'RUR')(U'2R)Uy  
        - 棱块在第三层，右侧红色在上方12点方向  
        - 棱块在第三层，右侧红色在上方3点方向  
        - 棱块在第三层，正面蓝色在上方6点方向  
        - 棱块在第三层，正面蓝色在上方9点方向  
            公式19:(RU'R'U2)(RUR')  
            逆公式:(RU'R')(U'2RUR')
        - 棱块在第三层，正面蓝色在上方12点方向  
            公式20:U(RU'U')(R'URU'R')  
            逆公式:(RUR'U'R)(UUR')U'  
        - 棱块在第三层，正面蓝色在上方3点方向  
    * 角块在第三层，正面色(蓝色)在上方  
        - 棱块在第二层，颜色正确  
            公式22:U'(RU'R'U2)(RU'R')
        - 棱块在第二层，颜色错误  
            公式23:U'(RUR')y'(UR'U'R)  
            逆公式:(R'URU')y(RU'R')
        - 棱块在第三层，右侧红色在上方6点方向  
            公式24:y'U(R'URU')(R'U'R)  
            逆公式:(R'UR)(UR'U'R)U'y
        - 棱块在第三层，右侧红色在上方9点方向  
        - 棱块在第三层，右侧红色在上方12点方向  
        - 棱块在第三层，右侧红色在上方3点方向  
            公式27:y'(RU'U')(R'2U')(R2U'R')
        - 棱块在第三层，正面蓝色在上方6点方向  
        - 棱块在第三层，正面蓝色在上方9点方向  
            公式29:U'(RU'U')(R'U2)(RU'R')  
            逆公式:(RUR')(U'2R)(UUR')U  
        - 棱块在第三层，正面蓝色在上方12点方向  
        - 棱块在第三层，正面蓝色在上方3点方向  
            公式31:URU'R'  
            逆公式:RUR'U'  
    * 角块在第三层，右面色(红色)在上方  
        - 棱块在第二层，颜色正确  
            公式32:U'(RU'U'R'U)(RUR')  
            逆公式:(RU'R')(U'RUUR')U  
        - 棱块在第二层，颜色错误  
        - 棱块在第三层，右侧红色在上方6点方向  
            公式34:y'U'(R'UR)  
            逆公式:(R'U'R)Uy
        - 棱块在第三层，右侧红色在上方9点方向  
            公式35:y'U(R'U'RU'U')(R'UR)  
            逆公式:(R'U'R)(UUR'UR)U'y  
        - 棱块在第三层，右侧红色在上方12点方向  
            公式36:y'U(R'U2RU'U')(R'UR)  
            逆公式:(R'U'R)(UUR'U'2R)U'y
        - 棱块在第三层，右侧红色在上方3点方向  
            公式37:(RU'R'U2)y'(R'U'R)  
            逆公式:(R'UR)y(U'2RUR')  
        - 棱块在第三层，正面蓝色在上方6点方向  
        - 棱块在第三层，正面蓝色在上方9点方向  
            公式39:U'(RUR'U)(RUR')  
            逆公式:(RU'R')(U'RU'R')  
        - 棱块在第三层，正面蓝色在上方12点方向  
            公式40:RUR'  
            逆公式:RU'R'
        - 棱块在第三层，正面蓝色在上方3点方向  
5. OLL  

6. PLL  