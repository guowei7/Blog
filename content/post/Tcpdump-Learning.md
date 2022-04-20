---
title: "Tcpdump Learning"
date: 2021-07-23T15:57:09+08:00
Description: "记录常用的tcpdump命令"
Tags: [
    "shell"
    ]
Categories: [
    "Linux"
    ]
DisableComments: false
---

主要记录常用的tcpdump命令
<!--more-->
### 常用的TCPDUMP命令  
1. 抓取所有网络包，并在terminal中显示抓取的结果，将包以十六进制的形式显式。  
    
    >tcpdump

2. 抓取所有的网络包，并存到result.pcap文件中。  

    >tcpdump -w result.pcap

3. 抓取所有的经过eth0网卡的网络包，并存到result.pcap文件中。  
    
    >tcpdump -i eth0 -w result.pcap

4. 抓取源地址是192.168.1.100的包，并将结果保存到result.pacp文件中。  

    >tcpdump src host 192.168.1.100 -w result.pcap

5. 抓取地址包含192.168.1.100的包，并将结果保存到result.pcap文件中。  
    
    >tcpdump host 192.168.1.100 -w result.pcap

6. 抓取目的地址包含192.168.1.100的包，并将结果保存到result.pcap文件中。  

    >tcpdump dest host 192.168.1.100 -w result.pcap

7. 抓取包含192.168.1.0/24网段的数据包  

    >tcpdump -i eth0 -vnn net 192.168.1.0/24

8. 抓取网卡eth0上所有包含端口22的数据包
    
    >tcpdump -i eth0 -vnn port

9. 抓取指定协议格式的数据包，协议格式可以是[udp，iccmp，arp，ip]中的任何一种  

    >tcpdump udp -i eth0 -vnn



