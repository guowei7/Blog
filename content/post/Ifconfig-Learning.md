---
title: "Ifconfig Learning"
date: 2022-01-12T11:26:59+08:00
Description: "记录常用的ifconfig命令"
Tags: ["shell"]
Categories: ["Linux"]
DisableComments: false
---

主要记录常用的ifconfig命令
<!--more-->
### 常用的ifconfig命令  
+ 网卡状态显示  

	1. 显示所有的网卡信息，包含down状态的网卡  
	
		>ifconfig -a
		
	2. 显示网卡的简短信息  

		>ifconfig -s  

	3. 显示网卡的详细信息  

		>ifconfig -v
		
+ 网卡开关  

	1. 网卡开启  

		>ifconfig interface up

	2. 网卡关闭  

		>ifconfig interface down

+ 网卡地址配置  

	1. 添加ip地址  

		>ifconfig interface add addr/prefixlen

	2. 删除ip地址  

		>ifconfig interface del addr/prefixlen

+ 网卡属性修改

	1. 开启和关闭网卡的arp协议  

		>ifconfig interface [-]arp  

	2. 开启和关闭网卡的混杂模式,混杂模式下所有报文都会被接收  

		>ifconfig interface [-]promisc  

	3. 开启和关闭allmulti，开启后所有组播包都会被接收  

		>ifconfig interface [-]allmulti  

	4. 修改MTU(Maximum Transfer Unit)的大小  

		>ifconfig interface mtu N

查看其他更加详细的信息请使用ifconfig --help命令  
