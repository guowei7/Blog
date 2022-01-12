---
title: "IgmpSnoop Learning"
date: 2021-07-21T09:46:28+08:00
Description: ""
Tags: [
    “二层组播”
    ]
Categories: [
    "计算机网络"
    ]
DisableComments: false
---

主要记录IGMP Snoop的学习
<!--more-->
### 1.IGMP Snoop简介  
1. 解决的问题  
    当组播源侧的Router将组播报文转发至Switch后，Switch将报文转发给组播用户。由于组播报文的目的地址为组播组地址，二层设备上学习不到这一类MAC表项，这样组播报文会在所有接口进行广播，这样所有成员都收到组播报文。  
    而使能IGMP Snoop功能后，Switch会侦听主机和上游三层设备之间交互的IGMP报文，通过分析报文中携带的信息，建立和维护二层组播转发表，从而指导组播数据在数据链路层按需转发。  

2. 基本概念  
    + 相关端口  

        |端口角色|作用|如何生成|
        |:--:|:--:|:--:|
        |路由器端口(上联口)，指二层组播设备上朝向组播路由器的接口。|二层设备通过此端口将report/lea    ve报文发送到三层组播设备。二层组播设备从此接口接收组播数据。|动态生成。收到IGMP查询报文的接   口都将被视为动态路由器端口;静态配置。|   
        |成员端口|二层组播设备往此接口发送组播数据|动态生成。收到IGMP  Report报文的接口都将标识为动态成员端口;静态配置。|
    
    + 组播地址范围  
        * MAC地址范围  
            01:00:5E:00:00:00/25
        * IP地址范围  
            224.0.0.0/4(224[1110 0000].0.0.0-239[1110 1111].255.255.255)
        * IP到MAC的转换  
            01:00:5E:(0)+IP组播地址后23位映射到MAC地址中。  
            注意，IP组播地址设备号有28位，而映射到MAC中的只有23位，故高5位未做映射，会存在多个IP映射同一个MAC地址的可能。  

    + 报文格式  
        * v1报文  
        * v2报文  
        * v3报文  

    + 基本工作机制  
        * Report机制  
        * Leave机制  
        * 通用查询机制  
        * 代理工作机制  

    + V3增强功能  
        * 源过滤功能  
        * SSM Mapping功能  
    
    + IGMP版本说明  
        * IGMPv1  
        * IGMPv2    
        * IGMPv3  

3. IGMP Snoop转发  
    + MAC转发模式  
        MAC转发为二层转发，因此只能在同一个vlan内转发，根据(VID+组播MAC+接口)。当配置为MAC转发模式时，软表显式出来的依然是IP，但是在下硬件的时候会按照IP到MAC地址转换规则下发MAC转发表。  
        此模式多个IP可能对应同一个MAC，例如：225.0.0.1和226.0.0.1。  

    + IP转发模式  
        IP转发为三层转发，可以跨VLAN转发，根据(IN_VID+GROUP+SOURCE+OUT_VID+接口)转发。  
        IP转发在软件中，可通过命令行配置是否需要跨VLAN转发，默认不开启跨VLAN。  

4. IGMP Snoop典型配置  
    + 单台单上联，MAC或IP转发  
        ```shell
        #创建相关VLAN(组播VLAN)
        vlan 100
        #未知组播丢弃
        unknown-multicast vlan 100 drop

        #全局使能IGMP SNOOP
        igmp-snooping start

        #创建并进入组播VLAN节点
        igmp-snooping mvlan 100

        #配置转发模式
        igmp-snooping forwarding-mode ip
        igmp-snooping forwarding-mode mac

        #配置上联口
        igmp-snooping uplink-port gigaethernet 1/0/3

        #接口加入组播VLAN并使能协议
        interface gigaethernet 1/0/1
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
        
        interface gigaethernet 1/0/2
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable

        interface gigaethernet 1/0/3
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
        ```
    + 单台单上联，跨VLAN转发  
        ```shell
        #创建相关VLAN
        vlan 10,20,100
        unknown-multicast vlan 100 drop

        #全局使能
        igmp-snooping mvlan 100

        #配置转发模式
        igmp-snooping forwarding-mode ip

        #使能跨VLAN组播复制
        igmp-snooping multicast-vlan enable

        #配置跨VLAN组播用户VLAN(出口VLAN)
        igmp-snooping multicast-vlan user-vlan 10,20

        #配置上联口
        igmp-snooping uplink-port gigaethernet 1/0/3

        #用户端口加入用户VLAN并使能组播协议
        interface gigaethernet 1/0/1
            port hybrid vlan 10 untagged
            port hybrid pvid 10
            igmp-snooping enable

        interface gigaethernet 1/0/2
            port hybrid vlan 20 untagged
            port hybrid pvid 20
            igmp-snooping enable

        #上联口加入组播VLAN并使能组播协议
        interface gigaethernet 1/0/3
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
        ```
    + 单台单上联，MAC或IP转发，代理模式  
        ```shell
        #创建相关VLAN(组播VLAN)
        vlan 100
        unknown-multicast vlan 100 drop

        #全局使能
        igmp-snooping start

        #创建并进入组播VLAN节点
        igmp-snooping mvlan 100

        #配置转发模式
        igmp-snooping forwarding-mode ip
        igmp-snooping forwarding-mode mac

        #配置上联口
        igmp-snooping uplink-port gigaethernet 1/0/3

        #配置代理模式
        igmp-snooping workmode igmp-proxy
        igmp-snooping report-suppress enable

        #配置代理IP(上游设备为IGMP时，必须配置通网段IP地址)
        igmp-snooping proxy-ip 10.1.1.1

        #配置查询使能(工程上使用代理模式必须使能，否则会导致表项老化)
        igmp-snooping querier enable

        #接口加入组播VLAN并使能协议
        interface gigaethernet 1/0/1
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable

        interface gigaethernet 1/0/2
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable

        interface gigaethernet 1/0/3
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
        ```
    + 单台单上联，跨VLAN转发，代理模式  
        ```shell
        #配置相关VLAN(组播VLAN)
        vlan 10,20,100
        unknown-multicast vlan 100 drop

        #全局使能
        igmp-snooping start

        #创建并进入组播VLAN节点
        igmp-snooping forwarding-mode ip

        #使能跨VLAN组播复制
        igmp-snooping multicast-vlan enable

        #配置跨VLAN组播用户VLAN(出口VLAN)
        igmp-snooping multicast user-vlan 10,20

        #配置上联口
        igmp-snooping uplink-port gigaethernet 1/0/3

        #配置代理模式
        igmp-snooping workmode igmp-proxy
        igmp-snooping report-suppress enable

        #配置代理IP(上游设备为IGMP时，必须配置同网段IP地址)
        igmp-snooping proxy-ip 10.1.1.1

        #配置查询器使能
        igmp-snooping querier enable

        #用户端口加入用户VLAN并使能组播协议
        interface gigaethernet 1/0/1
            port hybrid vlan 10 untagged
            port hybrid pvid 10
            igmp-snooping enable

        interface gigaethernet 1/0/2
            port hybrid vlan 20 untagged
            port hybrid pvid 20
            igmp-snooping enable

        #上联口加入组播VLAN并使能组播协议
        interface gigaethernet 1/0/3
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
        ```
    + 级联，上游同VLAN转发，下游跨VLAN转发  
        - DUT1配置  
            ```shell
            vlan 100
            unknown-multicast vlan 100 drop

            igmp-snooping start
            igmp-snooping mvlan 100
            igmp-snooping uplink-port gigaethernet 1/0/3

            interface gigaethernet 1/0/1
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/2
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/3
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable
            ```
        - DUT2配置
            ```shell
            vlan 10,100
            unknown-multicast vlan 100 drop

            igmp-snooping start
            igmp-snooping mvlan 100
            igmp-snooping uplink-port 1/0/2
            igmp-snooping forwarding-mode ip 
            igmp-snooping multicast-vlan enable
            igmp-snooping multicast user-vlan 10

            interface gigaethernet 1/0/1
                port hybrid vlan 10 untagged
                port hybrid pvid 10
                igmp-snooping enable

            interface gigaethernet 1/0/2
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable
            ```
        - DUT3配置
            同DUT2。  
    + 环网，不跨VLAN
        该拓扑需要配置环网协议，防止成环。    
        - DUT1配置  
            ```shell
            vlan 100
            unknown-multicast vlan 100 drop

            igmp-snooping start
            igmp-snooping mvlan 100
            igmp-snooping uplink-port gigaethernet 1/0/3

            interface gigaethernet 1/0/1
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/2
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/3
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable
            ```
        - DUT2配置  
            ```shell
            vlan 100
            unknown-multicast vlan 100 drop

            igmp-snooping start
            igmp-snooping mvlan 100
            igmp-snooping querier enable(环网的查询器视实际需求是否开启)

            interface gigaethernet 1/0/1
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/2
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/3
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable
            ```
        - DUT3配置
            同DUT2。  
    + 环网，跨VLAN  
        - DUT1配置  
            ```shell
            vlan 100
            unknown-multicast vlan 100 drop

            igmp-snooping start
            igmp-snooping mvlan 100
            igmp-snooping uplink-port gigaethernet 1/0/3

            interface gigaethernet 1/0/1
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/2
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/3
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable
            ```
        - DUT2配置  
            ```shell
            vlan 10,100
            unknown-multicast vlan 100 drop

            igmp-snooping start
            igmp-snooping mvlan 100
            igmp-snooping forwarding-mode ip
            igmp-snooping multicast-vlan enable
            igmp-snooping multicast user-vlan 10
            igmp-snooping querier enable

            interface gigaethernet 1/0/1
                port hybrid vlan 10 untagged
                port hybrid pvid 10
                igmp-snooping enable

            interface gigaethernet 1/0/2
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable

            interface gigaethernet 1/0/3
                port hybrid vlan 100 untagged
                port hybrid pvid 100
                igmp-snooping enable
            ```
        - DUT3配置
            配置和DUT2相同  
    + 可控组播配置  
        ```shell
        vlan 100
        unknown-multicast vlan 100 drop

        igmp-snooping start
        igmp-snooping mvlan 100
        igmp-snooping uplink-port gigaethernet 1/0/3

        #创建可控组播频道,source-address暂时没用到，填0即可
        igmp-control channel channel-1 mvlan 100 group-address 255.1.1.1 source-address 0.0.0.0
        igmp-control channel channel-2 mvaln 100 group-address 255.1.1.2 source-address 0.0.0.0
        igmp-control channel channel-3 mvlan 100 group-address 255.1.1.3 source-address 0.0.0.0
        igmp-control channel channel-4 mvaln 100 group-address 255.1.1.4 source-address 0.0.0.0

        #创建可控组播预览模板,总预览时间为60秒
        igmp-control preview-profile pp-1 time-total 60
        #创建非总时方式预览模板，预览次数3次，单次预览时间60s，两次预览时间的间隔为60s
        igmp-control preview-profile pp-2 time-sharing count 3 duration 60 interval 60

        #创建两个节目包
        igmp-control package pkg-1 channel channel-1 watch
        igmp-control package pkg-1 channel channel-2 preview pp-1
        igmp-control package pkg-1 channel channel-3 deny
        igmp-control package pkg-1 channel channel-4 preview pp-2

        igmp-control package pkg-2 channel channel-1 preview pp-1
        igmp-control package pkg-2 channel channel-2 preview pp-2
        igmp-control package pkg-2 channel channel-3 watch
        igmp-control package pkg-2 channel channel-4 deny


        interface gigaethernet 1/0/1
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
            #创建基于端口的检权用户，并绑定节目包pkg-1
            igmp-snooping ctrlmode enable
            igmp-control auth package pkg-1

        interface gigaethernet 1/0/2
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
            #创建基于端口的鉴权用户，并绑定节目包pkg-2
            igmp-snooping ctrlmode enable
            igmp-control auth package pkg-1

        interface gigaethernet 1/0/3
            port hybrid vlan 100 untagged
            port hybrid pvid 100
            igmp-snooping enable
        ```

        ```shell
        #查看频道表
        show igmp-control channel

        #查看预览模板表
        show igmp-control preview-profile

        #查看节目包表
        show igmp-control package

        #可控用户表
        show igmp-control interface user

        #用户预览参数复位，配置此命令后，所有用户的预览参数将恢复到初始值。
        reset igmp-control preview-profile
        #每天自动复位的时间
        reset igmp-control preview-reset-time 22:00

        #强制用户下线
        igmp-control group-address 225.1.1.1 force-leave
        ```
    
### 2. IGMP Snoop故障排查  
1. 常用故障定位命令  
    + 查看igmpsnoop全局参数  

        >show igmp-snooping

    + 查看igmpsnoop组播转发表  

        >show igmp-snooping egress-port

    + 查看igmpsnoop使能的端口  

        >show igmp-snooping interface

    + 查看igmpsnoop上联接口  

        >show igmp-snooping uplinkport

    + 查看igmpsnoop mvlan信息  

        >show igmp-snooping mvlan

    + 查看igmpsnoop组播组信息

        >show igmp-snooping group  

    + 开启igmpsnoop debug  
        
        >debug igmpsnoop  

2. 配置问题  

3. 流量转发问题  

4. 功能问题
    
