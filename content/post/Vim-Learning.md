---
title: "Vim Learning"
date: 2020-10-11T17:08:05+08:00
Description: "简单的记录VIM学习"
Tags: ["编辑器"]
Categories: ["Linux"]
DisableComment: false
---

本文主要介绍一些VIM的概念和基本使用
<!--more-->
### 1. Vim简介  
#### Vim工作模式  
- 命令模式(command-mode)  
    命令模式则多用于操作文本文件(而不是操作文本文件的内容),例如保存文件；或者用来改变编辑器本身的状态，例如设定多栏窗口、标签或者退出编辑器。  
- 插入模式(insert-mode)  
    插入模式则用来向文本中添加内容。
- 可视模式(visual-mode)  
    可视模式相当于高亮选取文本后的普通模式。  
    可视模式具有子模式，以行为单位进行选取的可视行模式，使用"V"键进入；和以块为单位进行选取的可视化模式，使用"Ctrl+v"键进入。
- 正常模式(normal-mode)  
    正常模式主要用来浏览和修改文本内容的。  
----------------------------
>普通模式=>插入模式(i、a、o等)  
>普通模式=>命令模式(:)  
>普通模式=>可视模式(v或V)  
>其他模式=>普通模式(esc)  
----------------------------  

### 2. Vimtutor  
#### 1.1 Moving the cursor  
移动光标，h，j，k，l四个按键的功能如下：  

- h: 向左移动光标  
- l: 向右移动光标  
- j: 向下移动光标  
- k: 向上移动光标  

#### 1.2 Exiting vim
进入命令模式使用q进行退出，其不会进行内容的保存。  
#### 1.3 Text Editing
- Deletion  
    将光标移动到要进行删除的字符处，使用x按钮删除不想要的字符。
- Insertion  
    将光标移动到套进行插入的字符出，使用i(I到行首进行插入)进入插入模式并进行输入。  
- Appendding  
    将光标移动到要进行添加的字符处，使用a(A到行尾进行输入)进入插入模式并进行输入。
#### 2.1 Deletion commands
d motion[w,$,e]  

- dw:删除从光标开始到下一个单词前的所有内容  
- d$:删除从光标开始到行尾的所有内容  
- de:删除从光标开始到本单词结束的所有内容  

d count[1,2,10] motion 

- d2w:删除从光标开始到后续第三个单词首部前的所有内容  
- d2e:删除从光标开始到后续第二个单词尾部前的所有内容  
- d2$:删除从光标本行开始向后两行(包含本行)的内容  

count dd  
vim开发者认为删除行的命令用的比较频繁，故使用dd作为行删除命令
#### 2.2 Using a count for a motion
count[1,10,5] motion[w,$,e]  

- 2w:光标移动到后面第三个单词的第一个字母处  
- 2e:光标移动到后面第二个单词的最后一个字母处  
- 2$:光标移动到向下数第二行(包含本行)的行尾处  

#### 2.3 The undo command
- u:撤销上一次修改的动作  
- U:撤销本行的所有修改  
- CTRL+R:反撤销u进行的撤销  

#### 3.1 The put command
使用p将上一次删除的文本添加到光标后  
#### 3.2 The replace command
使用r[x]来将光标处的字母替换成输入的[x]字母  

#### 3.3 The change operator
- ce:修改光标到单词尾部分的内容  
- c count motion:修改多个单词的内容  

#### 4.1 Cursor location and file status
- CTRL+G:用来显示光标在文件中的位置及文件的状态  
- G:移动光标到文件底部  
- gg:移动光标到文件顶部  
- count G:移动到文件中对应的count行  

#### 4.2 The search command
在/后面输入要进行查找的内容，键入n表示继续进行查找，键入N则表示反向查找。  
如果想要向上进行查找则是用？代替/进行查找。  
#### 4.3 Matching parentheses search
将光标移动到任何一个括号处，键入%号，则光标会移动到其匹配的括号处。  
#### 4.4 The Substitute command
键入:s/old/new/g，使用new来替换old  

- :s/old/new: 将当前行中的第一个old替换成new  
- :s/old/new/g: 将当前行中的全部old替换成new  
- :#,#s/old/new/g: 将#到#行的全部old替换成new  
- :%s/old/new/g: 将整个文件的old替换成new  
- :%s/old/new/gc: 带提示的将整个文件的old替换成new  

#### 5.1 How to execute an external command
在":!"后面接一个外部的命令(如:ls)即可以去执行该命令  
#### 5.2 More on writing files
使用":w filename"将当前的文件内容写入filename文件中  
#### 5.3 Selecting text to write
使用v视图选择部分内容，然后键入":"，尾行会显示":<,>"，然后键入"w filename"即可将所选内容写入到filename文件中。  
#### 5.4 Retrieving and merging files
使用":r filename"插入文件中的内容  
同时可以将外部命令的输出值写入文件中，如":r !ls"
#### 6.1 Open&APPEND&REPLACE Command
- Open: 键入"o"可以在当前光标后面添加一行，键入"O"可以在当前光标前面添加一行，两者皆可进入插入模式  
- Append: 键入"a"和"i"皆可在当前光标处进入插入模式进行输入，键入"A"和"I",分别实在光标所在行尾和行首处进入插入模式  
- Replace: 键入"R"即可在光标处进入插入模式，并是替换的方式进行文本修改  

#### 6.2 Copy and Paste text
先键入"v"进入可视模式，并选择要拷贝的文本，再使用"y"将其复制到粘贴板中。最后移动光标到需要粘贴的位置，键入"p"进行粘贴  
#### 6.3 Set option
- ":set ic"(ignore case)表示进行搜索时忽略大小写，":set noic"可取消该设置  
- ":set hls is"(highlight search,incsearch)表示将搜索内容进行高亮显示，":set nohls"表示取消该设置  
- 当只一次搜索进行大小写忽略时可以使用"\c"，如"/phare\c "  

#### 7.1 Getting help
Vim有三种方式打开在线帮助系统，分别是"HELP"键、"F1"键和键入":help"  
当打开另一个窗口后，可以使用CTRL-W、CTRL-W进行窗口间的跳转  
如果你想查询任何"子帮助"可以在":help"后面带上相应的参数如":help w"  
#### 7.2 Create a Startup Script
vim相较于Vi来说有更多的功能，但是绝大多数功能默认都是关闭的。要启用这些功能你必须创建一个"vimrc"文件。  

- 当没有vimrc文件时进行文件的创建，主要依赖于你的操作系统  
    + Unix :e ~/.vimrc  
    + Windows :$VIM/_vimrc  
- 读取示例"vimrc"文件的内容，添加到上一步创建的文件中 
    :r $VIMRUNTIME/vimrc_example.vim  
- 保存文件  
    :w  
这样当你第二次使用Vim时，就会有各种语法高亮  

#### 7.3 COMPLETION
使用CTRL-D和tab键进行命令的补全  
CTRL-D会以列表的形式将"e"开头的命令显示出来  
tab则会将命令进行补全  
### 3. 进一步学习vim
可以使用:help user-manual进行进一步的学习vim
