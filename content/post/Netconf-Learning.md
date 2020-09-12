---
title: "Netconf Learning"
date: 2020-09-08T17:15:38+08:00
Description: "网络配置命令学习的记录"
Tags: ["网络配置"]
Categories: ["计算机网络"]
DisableComments: false
---
主要记录网络配置命令学习的心得
<!--more-->
### 常用的网络配置命令  
1. 进入网络配置命令行  
    * 从host主机进入  
        ``` shell
        [root@vfowind ~]telnet 0 20000  
        ```  
    * 从主控盘进入  
        ``` shell  
        #登录主控盘
        [root@vfowind ~] docker-login mpu_0_1
        [root@mpu_0_1 /] telnet 0 9000  
        ```  
2. 展示网络接口分配的ip地址和给接口分配ip地址  
    * 展示网络接口的状态  
        ``` shell  
        #进入config视图，简写为con t
        [VFOSWIND] config terminal  
        #展示网络接口的状态
        [VFOSWIND(config)] show ip interface brief  
        ```  
    * 给接口分配ip地址  
        ``` shell  
        #进入需要配置的接口，如loopback0  
        [VFOSWIND(config)] interface loopback 0  
        #给loopback0配置ip地址1.1.1.55
        [VFOSWIND(loopback 0)] ip address 1.1.1.55/32  
        ```  
3. 配置直连接口  
    当两个设备的接口通过物理线连接后，并给两端的接口配置了同一个网段的ip后即会直连。  
    ``` shell  
    #主机A进入以太网接口eth-1gi 0/3/1/1 并配置ip地址为12.12.12.1/24
    [VFOSWIND(config)] interface eth-1gi 0/3/1/1  
    [VFOSWIND(eth-1gi 0/3/1/1)] ip address 12.12.12.1/24  
    [VFOSWIND(eth-1gi 0/3/1/1)] no shutdown  
    [VFOSWIND(eth-1gi 0/3/1/1)] commit
    ```  
    ``` shell  
    #主机B进入以太网接口eth-1gi 0/3/1/1 并配置ip地址为12.12.12.2/24
    [VFOSWIND(config)] interface eth-1gi 0/3/1/1  
    [VFOSWIND(eth-1gi 0/3/1/1)] ip address 12.12.12.2/24  
    [VFOSWIND(eth-1gi 0/3/1/1)] no shutdown
    [VFOSWIND(eth-1gi 0/3/1/1)] commit  
    ```  
4. 配置OSPF协议  
    给需要运行OSPF协议的路由器配置并将对应端口通告给OSPF学习  
    ``` shell  
    #运行OSPF，且进程号为1
    [VFOSWIND(config)] router ospf 1  
    #配置ospf的router-id
    [VFOSWIND(ospf)] router-id 1.1.1.57  
    #配置OSPF的area id，设置为0(可以整数形式，也可以ipv4的地址形式)
    [VFOSWIND(ospf)] area 0  
    #通告直连的端口，以建立邻居  
    [VFOSWIND(ospf-area)] interface eth-1gi 0/3/1/1
    [VFOSWIND(ospf-area-intf)] network broadcast    
    #将loopback0进行通告OSPF进行学习
    [VFOSWIND(ospf-area)] interface loopback 0  
    #通告方式
    [VFOSWIND(ospf-area-intf)] network point-to-point    
    [VFOSWIND(ospf-area-intf)] commit  
    ```  
5. 配置BGP并将路由信息通告BGP进行传递  
    * 配置BGP  
        ``` shell  
        #进入bgp配置视图，并且本地AS为100
        [VFOSWIND(config)] router bgp 100
        #进入需要配置的IBGP邻居视图  
        [VFOSWIND(router-bgp)] neighbor 1.1.1.56  
        #配置邻居的AS号，以确定是IBGP还是EBGP邻居
        [VFOSWIND(bgp-neighbor)] remote-as 100  
        #用环回口建立IBGP邻居时，需要修改TCP的源IP，以保证能建立邻居
        [VFOSWIND(bgp-neighbor)] update-source 1.1.1.55
        [VFOSWIND(bgp-neighbor)] no shutdown    
        [VFOSWIND(bgp-neighbor)] commit  

        #进入邻居视图的地址族配置
        [VFOSWIND(bgp-neighbor)] address-family ipv4 unicast  
        #配置EBGP邻居传给IBGP邻居时修改下一跳为自己
        [VFOSWIND(bgp-neighbor-af-ipv4-uni)] next-hop-self
        [VFOSWIND(bgp-neighbor-af-ipv4-uni)] commit】

        #进入需要配置的EBGP邻居视图
        [VFOSWIND(router-bgp)] neighbor 100.1.1.2
        [VFOSWIND(bgp-neighbor)] remote-as 200  
        [VFOSWIND(bgp-neighbor)] no shutdown    
        [VFOSWIND(bgp-neighbor)] commit  

        ```  
    * 通告路由给BGP协议  
        ``` shell  
        #根据要通告的路由类型选定对应的地址族，并进入配置视图
        [VFOSWIND(router-bgp)] address-family ipv4 unicast  
        #使用network通告对应的网段
        [VFOSWIND(bgp-af)] network 12.12.11.1/24  
        #使用redistribute命令将ospf 1的所有路由进行通告，还可以通告static isis connected类型的路由
        [VFOSWIND(bgp-af)] redistribute ospf 1  
        [VFOSWIND(bgp-af)] commit  
        ```
6. 保存路由设备的所有配置  
    ``` shell  
    #将配置保存到13.cfg中
    [VFOSWIND] save 13.cfg  
    ```