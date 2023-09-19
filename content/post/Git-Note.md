---
title: "Git Note"
date: 2023-07-25T14:54:25+08:00
Description: "git学习总结"
Tags: ["版本控制"]
Categories: ["Linux"]
DisableComments: false
---

git简单学习与总结  

<!--more-->
### 1. Git基础
#### 获取Git仓库
* 在已存在目录中初始化仓库  
``` git
$git init
```

* 克隆现有仓库  
``` git
$git clone <URL> [local-name]
```

#### 记录每次更新到仓库
* 检查当前文件状态  
``` git
$git status
```

* 跟踪新文件  
``` git
$git add [filename]
```

* 暂存已修改的文件  
``` git
$git add [filename]
```

* 状态简览  
``` git
$git status -s
```

* 忽略文件  
可以创建一个名为".gitignore"的文件，列出要忽略的文件的模式。  
```
# 忽略.o或.a结尾的文件
*.[oa]
# 忽略~结尾的文件
*~
```

* 查看已暂存和未暂存的修改  
``` git
# 查看修改之后还没有暂存起来的变化内容
$git diff
# 查看已暂存文件与最后一次提交的文件差异
$git diff --staged
```

* 提交更新  
``` git
$git commit
$git commit -m [info]
```

* 跳过使用暂存区域  
``` git
$git commit -a
```

* 移除文件  
``` git
# 从暂存区域移除文件并且连带从工作目录中删除指定的文件
$git rm [filename]
# 从暂存区域移除文件但是不从工作目录中删除指定的文件
$git rm --cached [filename]
```

* 移动文件  
``` git
$git mv file_from file_to
```

#### 查看提交历史
``` git
$git log
```

* 常用选项  

|选项|说明|
|:-:|:-:|
|-p|按补丁格式显示每个提交引入的差异|
|--stat|显示每次提交的文件修改统计信息|
|--shortstat|只显示--stat中最后的行署修改添加移除统计|
|--name-only|仅在提交信息后显示已修改的文件清单|
|--name-status|显示新增、修改、删除的文件清单|
|--abbrev-commit|仅显示SHA-1校验和所有40个字符中的前几个字符|
|--relative-date|使用较短的相对时间而不是完整格式显示日期|
|--graph|在日志旁以ASCII图形显示分支与合并历史|
|--pretty|使用其它格式显示历史提交信息。可用的选项包括oneline、short、full、fuller和format(自定义格式)|
|--oneline|--pretty=oneline --abbrev-commit和用的简写|

* 限制输出的选项  

|选项|说明|
|:-:|:-:|
|-\<n\>|仅显示最近的n条提交|
|--since,--after|仅显示指定时间之后的提交|
|--until,--before|仅显示指定时间之前的提交|
|--author|仅显示作者匹配指定字符串的提交|
|--committer|仅显示提交这匹配指定字符串的提交|
|--grep|仅显示提交说明中包含指定字符串的提交|
|-S|仅显示添加或删除内容匹配指定字符串的提交|

#### 撤销操作
* 修改commit  
``` git
$git commit --amend
```

* 取消暂存的文件  
``` git
$git reset HEAD <file>
```

* 撤销对文件的修改  
``` git
$git checkout -- <file>
```

#### 远程仓库的使用
* 查看远程仓库  
``` git
#origin git
$git remote
$git remote -v
```

* 添加远程仓库  
``` git
$git remote add <shortname> <url>
```

* 从远程仓库中抓取与拉取  
``` git
#只会将数据下载到你的本地仓库，不会自动合并或修改你当前的工作，必须手动将其合并入你的工作。
$git fetch <remote>

#当前分支设置了跟踪远程分支，那么可以用此命令自动抓取后合并该远程分支到当前分支。
$git pull 
```

* 推送到远程仓库  
``` git
$git push origin master
```

* 查看某个远程仓库  
``` git
$git remote show <remote>
```

* 远程仓库的重命名与移除  
``` git
$git remote rename <oldremote> <newremote>
$git remote remove <remote>
```


#### 打标签
* 列出标签  
``` git
$git tag
$git tag -l
```

* 创建标签  
    - 附注标签  
    ```
    #-m选项指定了一条将存储在标签中的信息
    $git tag -a v1.4 -m "my version 1.4"
    #查看标签信息和与之对应的提交信息
    $git show v1.4
    ```

    - 轻量标签  
    ``` git
    #轻量标签本质上是将提交校验和存储到一个文件中，没有保存任何其它信息。只需要提供标签名字
    $git tag v1.4-lw
    ```

* 后期打标签  
``` git
git tag -a v1.2 <commitId>
```

* 共享标签  
``` git
#将创建的<tagname>标签推送到共享服务器上
$git push origin <tagname>
#将所有不在远程仓库服务器上的标签全部传送到那里
$git push origin --tags
```

* 删除标签  
``` git
#删除一个标签
$git tag -d v1.4-lw
#要删除远程仓库上的标签使用下面两个方式
$git push origin :refs/tags/v1.4-lw
$git push origin --delete <tagname>
```

* 检出标签  
``` git
#此命令会让仓库当前处于头指针分离的状态
$git checkout <tagname>
#保留分离头状态的commit修改创建一个新分支
$git checkout -b <new-branch>
```

#### Git别名
可以通过git config文件来为命令设置一个别名 
``` git
$git config --global alias.co checkout
$igt config --global alias.br branch
$igt config --global alias.ci commit
$git config --global alias.st status

#恢复文件修改的命令别名为unstage
$git config --global alias.unstage 'reset HEAD --'
```

### 2. Git分支

#### 分支简介
git保存的是一个个提交对象(commit object)。  
当进行stage暂存操作时，git会为每一个文件计算校验和并保存此文件快照。当执行commit时，git会先计算每一个子目录的校验和然后保存为树对象，随后创建一个commit object提交对象。  
提交对象会包含基本信息、指向树对象的指针、只想上一次提交的(父对象)的指针。  

Git的分支，其实本质上仅仅是指向提交对象的可变指针。默认分支名字是master。
* 分支创建  
``` git
#创建新分支，git只是创建了一个可以移动的新指针。
$git branch <branchname>

#使用git log命令的--decorate查看各个分支当前锁指的对象)
$git log --oneline --decorate
```

git区分当前在哪一个分支上的方式很简单，存在一个名为HEAD的特殊指针。它是一个指针，指向当前所在的本地分支(将HEAD想象为当前分支的别名)。  

* 分支切换  
``` git
#将HEAD指向testing分支，且会将工作目录恢复成testing分支所指向的快照内容。
$git checkout testing
```

#### 分支的新建与合并
* 新建分支  
``` git
$git checkout -b <branchname>
#上述命令是以下两个命令的简写  
$git branch <branchname>
$git checkout <branchname>
```

* 分支的合并  
``` git
#先检出你想要合并入的分支
$git checkout master
#再将新的分支merge修改和入到master
$git merge <branchname>
#ibus
$git branch -d <branchname>
```

如果合并时\<branchname\>分支和master不在一条路径上(即产生了分歧),这个时候git会将两个分支最新的commit obj同两个分支共同的父commit obj三个一起形成一个合并commit obj，这个合并提交特别点在与他有不止一个父提交。  

* 遇到冲突时的分支合并  
当你在两个不同的分支中，对同一个文件的同一个部分进行了不同的修改，Git就没法干净的合并它们。  
只有手动解决冲突文件后使用git add \<filename\>将其标记为冲突已解决。一旦暂存这些原本有冲突的文件，Git就会将它们标记为冲突已解决。  

#### 分支管理
``` git
#查看分支
$git branch
#查看分支以及他最近的一次提交
$git branch -v
```

#### 分支开发工作流
* 长期分支  
* 主题分支  

#### 远程分支
远程引用是对远程仓库的引用，包括分支标签等等。
``` git
#获取远程引用的完整列表  
$git ls-remote <remote>
#获取远程分支的更多信息
$git remote show <remote>
```

``` git
#克隆时修改远程仓库名字
$git clone -o <remote>
#添加一个新的远程仓库
$git remote add <remote> <url>
```

* 推送
``` git
#推送本地分支来更新远程仓库上的分支
$git push <remote> <branch>
$git push <remote> <branch>:<remoteBranch>

#将远程分支的工作合并到当前分支
$git merge origin/serverfix
$检出orgin/serverfix并创建对应的本地分支serverfix
$git checkout -b serverfix origin/serverfix
```

* 跟踪分支  
``` git
#当本地不存在分支serverfix且存在一个匹配的<remote>/serverfix。则会创建serverfix本地分支并跟踪<remote>/serverfix
$git checkout serverfix
#设置当前本地分支的上游分支
$git branch -u origin/serverfix
#查看本地分支的上游分支以及状态
$git branch -vv
```

* 拉取  
大多数情况下可以理解成一个git fetch后执行一个git merge命令  

* 删除远程分支
``` git
$git push origin --delete serverfix
```

#### 变基
* 变基的基本操作  
``` git
$git checkout experiment
#将experiment变基到master分支上
$git rebase master
```
原理是先找到experiment和master的共同祖先，然后对比experiment当前节点和共同祖先之间的历次提交并存为临时文件。然后将experimnet指向master的目标基底并将之前的临时文件的修改依序应用。  
``` git
$git checkout master
#回到master分支，进行一次快进合并
$git merge experiment
```

这样进行一次快进合并后，master上就有experiment的修改了。  

* 更有趣的变基例子  
在对两个分支进行变基时，所生成的"重放"并不一定要在目标分支上应用，你也可以指定另外的一个分支进行应用。
``` git
#取出client分支，找出它从server分支分歧之后的补丁，然后把这些补丁在master分支上重放一遍，让client看起来像直接基于master修改一样
$git rebase --onto master server client
```

``` git
$git rebase master server
```

* 变基的风险  
如果提交存在于你的仓库之外，而别人可能基于这些提交进行开发，那么不要执行变基。  

* 用变基解决变基  
``` git
$git pull --rebase
```

如果你只对不会离开你电脑的提交执行变基，那就不会有事。如果你对已经推送过的提交执行变基，但别人没有基于它的提交，那么也不会有事。如果你对已经推送至共用仓库的提交上执行变基命令，并因此丢失一些别人的开发锁基于的提交，那就比较麻烦了。  

* 变基与合并  

### 3. 服务器上的Git
* 协议  
+ 本地协议  
+ HTTP协议  
+ SSH协议  
+ GIT协议  

* 在服务器上搭建git  
+ 把现有仓库导出为裸仓库  
``` git
$git clone --bare my_project my_project.git
```

+ 把裸仓库放到服务器上  
``` git
#将服务器上的my_project.git目录下运行如下命令，shared选项可以修改该仓库目录的组权限为可写。  
$git init --bare --shared
```

+ 小型安装  
    在主机上建立一个git账户，让每个需要写权限的人发送一个SSH公钥，然后将其加入git账户的~/.ssh/authorized_keys文件。这样一来，所有人都将通过git账户访问主机。  

* 生成SSH公钥  
``` shell
$ssh-kengen -t rsa -C [comment]
```

* 配置服务器  
``` shell
#服务器上创建一个系统用户git，并建立一个.ssh目录和authorized_keys文件
$sudo adduser git
$su git
$cd 
$mkdir .ssh && chmod 700 .ssh
$touch .ssh/authorized_keys && chmod 600 .ssh/authorized_keys
#添加开发者ssh公钥到authorized_keys中
#创建一个空仓库
$cd /srv/git
$mkdir project.git
$cd project.git
$git init --bareS
#修改git用户的普通shell为git-shell
$cat /etc/shells
$which git-shell
$sudo -e /etc/shells
$sudo chsh git -s $(which git-shell)
```

* Git守护进程  
``` git
$git daemon --reuseaddr --base-path=/srv/git/ /srv/git/
```

Linux中将该进程以守护进程的方式可以通过systemd。  
创建一个git-daemon.service并放在/etc/systemd/system/中，其内容如下:
```
[Unit]
Description=Start Git Daemon

[Service]
ExecStart=/usr/bin/git daemon --reuseaddr --base-path=/srv/git/ /srv/git/

Restart=always
RestartSec=500ms

StandardOutput=syslog
StandardError=syslog
SyslogIdentifier=git-daemon

User=git
Group=git

[Install]
WantedBy=multi-user.target
```

然后使用systemctl enable git-daemon让其在系统启动时自动运行。  

``` shell
#在仓库下创建git-daemon-export-ok文件来达到该仓库无需授权的项目访问服务
$cd /path/to/project.git
$touch git-daemon-export-ok
```
* Smart HTTP  
* GitWeb  
* GitLab  
* 第三方托管的选择  

### 4. 分布式Git
* 分布式工作流程  
    - 集中式工作流
    - 集成管理者工作流
    - 主管与副主管工作流
* 向一个项目贡献
* 维护项目  
    - 在主题分支中工作
    - 应用来自邮件的补丁
    ``` git
    #应用git diff创建的补丁
    $git apply /tmp/patch-ruby-client.patch

    #检查补丁是否可以顺利应用
    $git apply --check /tmp/pathc-ruby-client.patch

    #应用由format-patch命令生成的补丁
    $git am /tmp/patch-ruby-client.patch
    ```
    - 检出远程分支
    ``` git
    $git remote add jessica git://github.com/jessica/myproject.git
    $git fetch jessica
    $git checkout -b rubyclient jesssica/ruby-client
    ```
    - 确定引入了哪些东西
    ``` git
    #排除master中的提交进行历史查看
    $git log contrib --not master

    #查看contrib分支的最新提交和master与contrib共同祖先节点间的不同
    #git diff master...contrib
    ```
    - 将贡献的工作整合进来
    - 
### 5. GitHub

### 6. Git工具
### 7. 自定义Git
### 8. Git与其它系统
### 9. Git内部原理
### 10. Git命令