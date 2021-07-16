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

5. 配置ISIS协议  
    配置基础的ISIS进程，并使对应的接口使能。  

    ``` shell
    #配置isis进程，进程名为1  
    [VFOSWIND(config)] router isis 1  
    #配置网络实体名称，区域ID(1-13bytes)+系统ID(6bytes)+n-selector(00固定值)  
    [VFOSWIND(config-isis)] net 10.0000.0000.0001.00  
    [VFOSWIND(config-isis)] is-type level-1  
    #设置开销类型  
    [VFOSWIND(config-isis)] metric-style wide level-1  
    [VFOSWIND(config-isis)] metric-style wide level-2  
    #配置接口使能，将要加入isis的接口都进行相应地址族的使能  
    [VFOSWIND(config-isis)] interface loopback 0  
    [VFOSWIND(config-isis-if)] address-family ipv4 unicast  
    ```

6. 配置BGP并将路由信息通告BGP进行传递  
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

    * 路由延时下发  
        将bgp的路由延时下发给urm，并且只有当为frr或ecmp情况有多个下一跳时才会延时下发。
        ``` 
        router bgp 100
            bgp router-id 2.2.2.2
            route-select delay 60
        !
        ```

    * 默认local-preference值修改  
        ```
        router bgp 100
            bgp router-id 2.2.2.2
            #默认值为100,设置为100和no的反向命令是相同的效果
            bgp default local-preference 50 
        !
        ```

7. evpn相关配置  
    * evpn instance配置  

    > evpn  
    > &emsp;evpn vpn-instance 1  
    > &emsp;&emsp;rd 1:1  
    > &emsp;&emsp;route-target import 1:1  
    > &emsp;&emsp;route-target export 1:1  
    > &emsp;!  
    > &emsp;interface eth-1gi 0/3/1/4  
    > &emsp;&emsp;ethernet-segment  
    > &emsp;&emsp;&emsp;identifier  type 0 00.00.00.00.00.00.00.00.04  
    > &emsp;&emsp;!  
    > &emsp;!  
    > !  
    
    * 子接口配置  

    > interface eth-1gi 0/3/1/4.1  
    > &emsp;l2transport  
    > &emsp;encapsulation dot1q  
    > &emsp;vlan-type dot1q 200  
    > !  
    > interface eth-1gi 0/3/1/4.2  
    > &emsp;l2transport  
    > &emsp;encapsulation dot1q  
    > &emsp;vlan-type dot1q 200  
    > !  
    
    * 绑定evpn实例到子接口  

    > evpl 15  
    > &emsp;neighbor evpn vpn-instance 1  target 1 source 2  
    > &emsp;interface eth-1gi 0/3/1/4.1  
    > !  
    
    * vpls配置,并配置2类evpn路由，mac-address配置  

    > bridge-domain bid  
    > &emsp;evpn binding vpn-instance 1  
    > &emsp;interface eth-1gi 0/3/1/4.2  
    > &emsp;&emsp;mac-address static 0000-0001-0015  
    > !  
    
    * bgp中配置l2vpn evpn地址族  

    >router bgp 100  
    >&emsp;bgp router-id 1.1.1.1  
    >&emsp;address-family ipv4 unicast  
    >&emsp;&emsp;redistribute connected  
    >&emsp;&emsp;redistribute ospf 1  
    >&emsp;!  
    >&emsp;neighbor 2.2.2.2  
    >&emsp;&emsp;remote-as 100  
    >&emsp;&emsp;update-source 1.1.1.1  
    >&emsp;&emsp;address-family ipv4 unicast  
    >&emsp;&emsp;&emsp;send community extended  
    >&emsp;&emsp;!  
    >&emsp;&emsp;address-family l2vpn evpn  
    >&emsp;&emsp;!  
    >&emsp;!  
    >&emsp;neighbor 3.3.3.3  
    >&emsp;&emsp;remote-as 100  
    >&emsp;&emsp;update-source 1.1.1.1  
    >&emsp;&emsp;address-family ipv4 unicast  
    >&emsp;&emsp;&emsp;send community extended  
    >&emsp;&emsp;!  
    >&emsp;&emsp;address-family l2vpn evpn  
    >&emsp;&emsp;!  
    >&emsp;!  
    >!  

8. 路由设备配置相关  
    ``` shell  
    #将配置保存到13.cfg中
    [VFOSWIND] save 13.cfg  
    #加载配置，配置的路径为(/mnt/userdir/fosuserdir/rootfs/home/config/nem/ 主控盘中)，也可以使用find / -name filename 进行查找
    [VFOSWIND] startup saved-configuration 13.cfg
    #清空当前的配置，加载配置后需要重启主控盘(docker)生效
    [VFOSWIND] startup saved-configuration null
    ```

9. VPNV4相关配置  
    ``` shell
    #配置vrf，包括rd rt的相关配置
    [VFOSWIND(config)]vrf 1
    [VFOSWIND(vrf)]address-family ipv4 unicast
    [VFOSWIND(vrf-af-ipv4-uni)]rd 1:1
    [VFOSWIND(vrf-af-ipv4-uni)]route-target both 1:1
    #配置对应接口绑定vrf
    [VFOSWIND(config)]interface eth-1gi 0/3/1/4.1
    [VFOSWIND(eth-1gi 0/3/1/4.1)]ip vrf forwarding 1
    [VFOSWIND(eth-1gi 0/3/1/4.1)]ip address 192.168.3.1/24
    [VFOSWIND(eth-1gi 0/3/1/4.1)]encapsulation dot1q
    [VFOSWIND(eth-1gi 0/3/1/4.1)]vlan-type dot1q 100
    #将vrf的路由通告给BGP
    [VFOSWIND(config)]router bgp 100
    [VFOSWIND(router-bgp)]vrf 1
    [VFOSWIND(bgp-vrf)]address-family ipv4 unicast
    [VFOSWIND(bgp-vrf-af-ipv4-uni)]redistribute connected
    ```

10. 路由策略相关配置  
    ``` shell
    #配置ipv4-prefix
    [VFOSWIND(config)]ipv4-prefix 1 seq 1 deny prefix 192.168.3.1/24
    #配置rd-filter
    [VFOSWIND(config)]ip rd-filter filter-1 deny 5:5
    #配置route-policy
    [VFOSWIND(config)]route-policy policy_1_out deny node 1
    [VFOSWIND(route-policy)]if-match ipv4 address ipv4-prefix 1
    [VFOSWIND(route-policy)]if-match rd-filter filter-1
    #BGP应用对应的route-policy进行过滤。
    [VFOSWIND(config)]router bgp 100
    [VFOSWIND(router-bgp)]neighbor 2.2.2.2
    [VFOSWIND(bgp-neighbor)]address-family vpnv4 unicast
    [VFOSWIND(bgp-vrf-af-vpnv4-uni)]route-policy policy_1_out out 
    ```

11. 普通vpws和vpls的配置  
    ``` shell
    #配置二层子接口
    interface eth-1gi 0/3/1/4.1
        l2transport
        encapsulation dot1q
        vlan-type dot1q 100
    !
    #配置vpws
    vpws vpws1
        interface eth-1gi 0/3/1/4.1
        !
        peer 4.4.4.4 vcid 3 encapsulation tagged
        !
    !
    #配置vpls
    vfi vpls1
        vsi-id 100  #pw两端的vsi-id必须一致，否则vpls无法建立成功
        interface eth-1gi 0/3/1/4.100
        !
        peer 2.2.2.2 vcid 10 encapsulation tagged
        !
    !
    #配置ldp中的peer
    mpls ldp
        router-id 1.1.1.1
        interface eth-1gi 0/3/1/1
        !
        targeted-peer ipv4 2.2.2.2
        !
        targeted-peer ipv4 4.4.4.4
        !
    !
    ```
    
### 仪表常用配置

1. 连接仪表后，配置需要打流的端口port。  
2. 添加devices。  
    + 配置对应的source ip和gateway。
    + ARP/ND->start ARP/ND学习ARP。
3. 添加三层流。  
    + Add Bound Stream Block。
    + port选择，源端口->目的端口。
    + apply后，start ARP/ND进行学习arp。
4. 可以利用capture来进行抓报。  

