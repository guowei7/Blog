---
title: "Premiere Learning"
date: 2020-11-15T10:55:30+08:00
Description: ""
Tags: ["Premiere"]
Categories: ["Adobe"]
DisableComments: false
---

主要是Premiere官方文档的学习记录
<!--more-->
### 创建项目和导入剪辑

#### 创建项目

1. 使用***Open Project***来打开项目。  
2. 使用***New Project***来新建项目。  
3. 使用***Media Browser***(媒体浏览器)面板导入媒体文件。  

#### 工作区概述

1. 面板：每个面板都有特定的用途。可以调整面板的大小和位置。所有面板都可以在"Window"(窗口)菜单中找到。  
2. 工作区：显示特定任务(如编辑视频或处理音频)会用到的预设面板布局。要重置工作区，Window>Workspace>Reset to Saved Layout  
3. 蓝色边框：一次只会有一个面板被蓝色边框选中，表示该面板处于活动状态。  

#### 创建序列
Premiere Pro中的大部分编辑工作都是在创建编辑序列过程中完成的。在添加剪辑前，您首先需要创建序列。  
1. 创建序列，在同一个项目中，可以根据需要创建多个序列。  
2. 要创建序列，将一个或多个剪辑拖放到空的时间轴面板中，或者在项目面板中选择一个或多个剪辑，右键并选择***New Sequence from Clip***。  
3. 要添加更多剪辑，只需要将剪辑拖放到时间轴面板即可。  
4. 要从序列中删除剪辑，选中剪辑，然后按Backspace，若使用shift+Delete,则剪辑在删除后不会在序列上留下空隙。   

### 了解Premiere Pro面板  
#### 使用项目面板  

1. 可以在项目面板中将项目整理到不同的Bin中。  
2. 并非项目面板中的所有项目都要在序列中使用。  
3. 点击项目面板左下角的***List View***(列表视图)或***Icon View***(图标视图)按钮可分别查看剪辑的信息和剪辑的缩略图。  
4. 列表视图会按列显示每个项目的信息，如帧速率、媒体时长等。点击某列的标题可根据标题内容对剪辑进行排序。  
5. 在搜索栏中输入文本可以仅显示与输入文本匹配的剪辑。  

#### 素材监视器和节目监视器

1. 在序列中插入剪辑前，可以使用素材监视器来预览剪辑。你可以使用播放控件或空格键播放和暂停播放剪辑，也可以拖动监视器底部的蓝色播放头，快速查看剪辑中的特定部分。你可以点击入点标记和出点标记按钮，在剪辑中选择你要添加到序列中的特性部分。  
2. 节目监视器用于播放序列。节目监视器播放头与时间轴面板中的播放头是同步的。  

#### 了解时间轴面板

1. 时间轴上的时间为从左到右流动。播放头位于哪里，你就会在节目监视器中看到哪一帧。  
2. 你可以在不同的视频和音频轨道上放置剪辑。视频轨道以堆栈形式播放，当你在节目监视器中播放序列时，位于上面视频轨道中的剪辑会先于下面视频轨道的剪辑显示。音频轨道上的剪辑会一起播放，所以你可以将多个音频剪辑放在不同的轨道上，打造混音效果。  
3. 如果要处理序列的某个特定部分，你可以启用和禁用单个轨道。  
4. 时间轴面板中的剪辑选择非常重要，所以如果没有获得预期效果，可以检查一下是否没有选对剪辑。  
5. 时间轴面板和节目监视器面板中有很多工具，但是选择工具最常用，该工具位于工具面板顶部。  

### 剪辑技巧  
#### 创建新序列  

1. 要创建新序列，请点击项目面板中的***New Item***(新建项目)菜单，然后从下拉菜单中选择***Sequene***(序列)。根据规格选择预设设置，即使选择了错误的设置也没关系，当第一次添加剪辑到序列中时，可以选择调整设置来匹配剪辑。  
2. 也可以直接将剪辑拖放到项目面板中的***New Item***(新建项目)菜单上，这样可以根据剪辑的属性自动创建新序列。  
3. 可以在做项目面板中通过***List View***查看序列的帧尺寸和帧速率。  

#### 向序列中添加剪辑

1. 可以将项目面板中的剪辑拖放到时间轴面板中的轨道上，来构建序列。  
2. 可以直接将素材监视器中的剪辑片段拖放到时间轴面板中。  
3. 使用素材监视器底部的***Drag Video Only***或***Drag Audio Only***图标，可以单独将视频或音频拖动到序列中。  

#### 从序列中删除剪辑

1. 要删除剪辑并留出空隙，请选中剪辑并按***Backspace***  
2. 要删除剪辑不留空隙，请选中剪辑并按***Shift+Delete***  
3. 按住***Shift***的同时点击选择在多个剪辑。  
4. 使用***Track Select Forward Tool***选中从您点击的剪辑至序列末尾的所有剪辑。  

#### 移动序列中的剪辑

1. 要更改序列中剪辑的顺序，可以将剪辑拖放到新位置，这样会留下一个空隙，并且会覆盖新位置上的原有剪辑。   
2. 按住***Ctrl***的同时拖动剪辑，可以将剪辑插入新位置，将现有的剪辑向右推。  
3. 如果只想选择关键剪辑中的视频或音频部分，请右键剪辑并选择***Unlink***  
4. 你可以点击时间轴面板左上角的***Linked Selection***按钮，对整个序列打开或关闭视频和音频关联。  
5. 使用***Razor Tool***(剃刀工具)将剪辑切割为两部分，然后可以分别移动。  

#### 对音频进行简单调整

1. 可使用***Audio Clip Mixer***(混音器)面板更改音频剪辑的音量。  
2. 把时间轴的播放头放在你要调整的剪辑上。  
3. 上下拖动***Audio Clip Mixer***中与包含该剪辑的序列对应部分音量控制器。通常0dB表示最大音量。  
4. 使用***Mute Track***(静音轨道)和***Solo Track***(单轨)按钮可以选择性地听不同音频轨道上的声音。  
5. 可以使用混音器中的音频控制盘调整剪辑声音的平衡性。   

### 处理图形和标题







