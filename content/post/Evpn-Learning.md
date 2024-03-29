---
title: "Evpn Learning"
date: 2020-10-22T08:32:21+08:00
Description: "主要记录EVPN的学习"
Tags: ["EVPN"]
Categories: ["计算机网络"]
DisableComments: false
---

主要是对EVPN进行学习的笔记  
<!--more-->

### EVPN  
#### EVPN介绍  

* 定义  

    EVPN(Ethernet Virtual Private Network)是一种用于二层网络互联的VPN技术。EVPN技术采用类似于BGP/MPLS IP VPN的机制，通过扩展BGP协议，使用扩展后的可达性信息，使不同站点的二层网络间的MAC地址学习和发布过程从数据平面转移到控制平面。  

* 目的  

    随着网络技术的发展，用户间经常采用VPLS技术连接骨干网，从而实现业务互通，但是使用VPLS技术存在如下问题：  

    + 无法实现负载分担:VPLS不支持在多归网络中流量传输的负载分担；  
    + 网络资源的消耗较高：一方面，当需要实现大量的站点互联时，运营商骨干网上所有PE设备配置成全连接状态，即任意两台PE设备之间都要建立PW。当PE设备很多时，网络资源的消耗都会很大；另一方面，大量用来学习MAC地址的ARP报文不仅占用网络带宽，还可能造成远端站点学习到不必要的MAC地址，同时远端站点的主机对ARP请求报文的频繁处理也会造成对主机CPU资源的浪费。  

    与VPLS相比，EVPN技术可以解决上述问题：  

    + EVPN通过扩展BGP协议使二层网络间的MAC地址学习和发布过程从数据平面转移到控制平面。这样可以使设备在管理MAC地址时像管理路由一样，使目的MAC地址相同但下一跳不同的多条EVPN路由实现负载分担；  
    + 通过使用EVPN技术，运营商骨干网上的PE设备之间不再需要建立全连接。这是因为在EVPN网络中PE设备之间通过BGP协议实现互相通信的。BGP协议自带路由反射器功能，所以可以在运营商骨干网上部署路由反射器，所有PE设备与反射器建立邻居关系，通过路由反射器来反射EVPN路由，大大降低了网络复杂度，减少了网络信令数量；  
    + PE设备通过ARP协议和MAC/IP地址通告路由分别学习本地和远端的MAC地址信息以及其对应的IP地址，并将这些信息缓存至本地。当PE设备再收到其他ARP请求后，将先根据ARP请求中的目的IP地址查找本地缓存的MAC与IP地址的对应信息，如果查找到对应信息，PE将返回ARP响应报文，避免ARP请求报文向其他PE设备广播、减少网络资源消耗。  

* 受益  

    部署EVPN对于用户具有以下价值:  

    + 提高链路的利用率及传输效率：EVPN支持负载分担，可以合理利用网络资源，减少网络拥塞；  
    + 避免全连接造成网络资源的消耗：EVPN支持在公网上部署路由反射器，避免在公网上部署PE设备间的全连接，减少逻辑连接的数量；  
    + 减少ARP广播流量造成网络资源的消耗：PE设备通过本地缓存的MAC地址信息，可以对本地连接的站点发来的ARP请求进行响应，防止ARP请求的大范围广播。  

#### EVPN原理描述

* EVPN的典型组网  

    如EVPN组网示意图，EVPN网络和BGP/MPLS IP VPN的网络结构相似，为了实现各个站点(Site)之间的互通，运营商骨干网上的PE设备上建立EVPN实例并接入各个站点的CE设备，同时各个PE之间建立的邻居关系以及MPLS/SR隧道。但是EVPN网络与BGP/MPLS IP VPN网络的不同之处在于各个站点内是二层网络，因此从各个CE学习到的是MAC地址而不是路由，PE通过EVPN特有的路由类型将自己从CE学习到MAC地址转发到其他Site。  
    根据PE与CE间的连接形式，EVPN的组网类型可分为CE多归属和CE单归属两种类型。图中，CE1、CE2和CE4与PE的连接形式属于CE单归属组网类型，CE3与PE2、PE3连接的类型属于CE多归属组网类型。CE多归属组网类型可以支持负载分担功能。  
    EVPN技术为PE与某一CE的连接定义唯一的标识ESI(Ethernet Segment Identifier),连接同一CE的多个PE上的ESI值相同，连接不同CE的ESI值不同。PE之间进行路由传播时，路由中会携带ESI值使PE间可以感知到连接同一CE的其他PE设备。  
    ![EVPN组网示意图](../../images/EVPN-Learning/EVPN组网示意图.png)  

* EVPN路由  

    为了不同站点之间可以互相学习对方的MAC信息，因此EVPN在BGP协议的基础上定义了一种新的NLRI，被称为EVPN NLRI。EVPN NLRI中包含如下几种常用的EVPN路由类型：  

    + 以太自动发现路由(Ethernet Auto-Discovery Route)：当各个PE之间的BGP邻居关系建立成功后，PE之间会传递Type-1路由。Type-1路由可以向其他PE通告本端对接入站点的MAC地址的可达性，即PE对连接的站点是否可达。Type-1路由可以分为Ethernet Auto-Discovery Per ES路由和Ethernet Auto-Discovery Per EVI路由，前者主要用于快速收敛、冗余模式和水平分割，后者主要用于别名。以太自动发现路由的NLRI报文格式如下图：  
    ![以太自动发现路由](../../images/EVPN-Learning/以太自动发现路由.png)  
    字段解释如下：  

        - Route Distinguisher：Ethernet Auto-Discovery Per ES路由上该字段是由PE上设置的源IP地址组合而成，例如X.X.X.X：0;Ethernet Auto-Discovery Per EVI路由上该字段为EVPN实例下设置的RD值。  
        - Ethernet Segment Identifier: PE与某一CE的连接定义的唯一标识。  
        - Ethernet Tag ID： Ethernet Auto-Discovery Per ES路由上该字段为全F;Ethernet Auto-Discovery Per EVI路由上该字段用于标识一个ES下的不同的子广播域，全0标识该EVI只有一个广播域。  
        - MPLS Label: Ethernet Auto-Discovery Per ES路由上该字段全为0；Ethernet Auto-Discovery Per EVI路由上该字段为EVPN单播流量负载 分担转发时使用的MPLS标签。  
    + MAC/IP地址通告路由(MAC/IP Advertisement Route)：MAC/IP地址通告路由可以携带本端PE上EVPN实例的RD值、ESI值以及EVPN实例对应的私网标签，MAC/IP地址通告路由的NLRI报文格式如下图所示。除了NLRI，MAC/IP地址通告路由还包含了EVPN实例的RT值、路由下一跳等信息。该类型路由可以用于从本端PE向其它PE发布单播MAC/IP地址的可达信息。  
    ![MAC-IP地址通告路由](../../images/EVPN-Learning/MAC-IP地址通告路由.png)  
    字段解释如下  

        - Route Distinguisher：该字段为EVPN实例下设置的RD值。  
        - Ethernet Segment Identifier：PE与某一CE的连接定义的唯一标识。  
        - Ethernet Tag ID：该字段在EVPN VPWS场景中取值为本端Service ID，在VLAN-AWare接入BD EVPN场景中取值为BD-Tag，其他场景中全0。  
        - MAC Address Length：被该类型路由通告的MAC地址长度。  
        - MAC Address：被该类型路由通告的MAC地址。  
        - IP Address Length：被该类型路由通告的主机IP地址掩码长度。  
        - IP Address：被该类型路由通告的主机IP地址。  
        - MPLS Label1：二层业务流量转发使用的标签。  
        - MPLS Label2：三层业务流量转发使用的标签。  

        该类型路由在控制平面中的作用包括：  

        - 主机MAC地址通告  
            如果需要实现接入不同PE设备的主机间二层业务互通，则两台PE间需要互相学习主机MAC。作为BGP EVPN对等体的PE之间通过交换MAC/IP地址通告路由，可以相互通告已经获取到的主机MAC地址。其中MAC Address Length和MAC Address字段为主机MAC地址。  
        - 主机ARP通告  
            MAC/IP地址通告路由可以同时携带主机MAC地址+主机IP地址，因此该路由可以用来在PE之间传递主机ARP表项，实现主机ARP通告。其中，MAC Address和MAC Address Length字段为主机MAC地址，IP Address和IP Address Length字段为主机IP地址。此时的MAC/IP地址通告路由也称为ARP类型路由。
        - 主机IP路由通告  
            如果需要实现接入不同PE设备的IPv4主机间三层业务互通，则两台PE间需要互相学习IPv4路由。作为BGP EVPN对等体的PE之间通过交换MAC/IP地址通告路由，可以相互通告已经获取到的主机IPv4地址。其中，IP Address Length和IP Address字段为主机IP路由的目的地址，同时MPLS Label2字段必须携带三层业务流量转发使用的标签。此时的MAC/IP地址通告路由也称为IRB(Integrated Routing and Bridge)类型路由。  
        - 主机ND信息通告  
            MAC/IP地址通告路由可以同时携带主机MAC地址+主机IPv6地址，因此该路由可以用来在PE之间传递主机ND表项，实现主机ND信息通告。其中，MAC Address和MAC Address Length字段为主机IPv6地址。此时的MAC/IP地址通告路由也称为ND类型路由。
        - 主机IPv6路由通告  
            如果需要实现接入不同PE设备的IPv6主机间三层业务互通，则两台PE间需要互相学习主机IPv6路由。作为BGP EVPN对等体的PE之间通过交换MAC/IP地址通告路由，可以相互通告已经获取到的主机IPv6路由。其中，IP Address Length和IP Address字段为主机IPv6路由的目的地址，同时MPLS Label2字段必须携带三层业务流量转发使用的标签。此时的MAC/IP地址通告路由也称为IRBv6类型路由。  

    + 集成多播路由(Inclusive Multicase Route)：当PE之间的BGP邻居关系建立成功后，PE之间会传递集成多播路由，集成多播路由可以携带本端PE上EVPN实例的RD和RT值，以及Source IP(一般为本端PE的Loopback地址)和PMSI(Provider Multicast Service Interface)信息，其中PMSI用于携带多播报文传输所使用的隧道类型(Ingress Replication或者MLDP)和隧道标签信息。PMSI和RT值承载在路由的属性信息中，RD和Source IP承载在路由的NLRI信息中，集成多播路由的NLRI报文格式如下图。多播流量包括广播流量、组播流量和未知目的地址的单播流量。当一台PE设备收到多播流量后，会将多播流量以点到多点的形式转发给其他PE设备。PE设备之间通过集成多播路由可以建立传送流量的隧道。  
    ![集成多播路由](../../images/EVPN-Learning/集成多播路由.png)  
    字段解释如下:  

        - Route Distinguisher：该字段为EVPN实例下设置的RD值。  
        - Ethernet Tag ID：该字段在EVPN VPWS场景中取值为本端Service ID，在VLAN-Aware接入BD EVPN场景中取值为BD-Tag，其他场景中全0。  
        - IP Address Length：PE上配置的源地址长度。  
        - Originating Router's IP Address：PE上配置的源地址。  

    + 以太网段路由(Ethernet Segment Route)：以太网段路由可以携带本端PE的ESI值、Source IP地址和RD值，用来实现连接到相同CE的设备之间互相自动发现。以太网段路由主要用于DF(Designated Forwarder,指定转发者)选举。以太网段路由的NLRI报文格式如下图：  
    ![以太网段路由](../../images/EVPN-Learning/以太网段路由.png)  
    字段解释如下：  

        - Route Distinguisher：该字段是由PE上设置的源IP地址组合而成，例如X.X.X.X:0。  
        - Ethernet Segment Identifier：PE与某一CE的连接定义的唯一标识。  
        - IP Address Length：PE上配置的源地址长度。  
        - Originating Router's IP Address：PE上配置的源地址。  

    + IP前缀路由(IP Prefix Route)：IP前缀路由用于在网络中传播从接入侧网络收到的主机IP地址信息或主机IP所在的网段信息。该类型路由的NLRI报文格式如下图  
    ![IP前缀路由](../../images/EVPN-Learning/IP前缀路由.png)  
    字段解释如下：  

        - Route Distinguisher：该字段为EVPN实例下设置的RD值。  
        - Ethernet Segment Identifier：PE与某一CE的连接定义的唯一标识。  
        - Ethernet Tag ID：该字段当前仅支持各位置为0。  
        - IP Prefix Length：该字段为此路由携带的IP前缀掩码长度。  
        - IP Prefix：该字段为此路由携带的IP前缀地址。  
        - GW IP Address：该字段为默认网关地址。  
        - MPLS Label：三层业务流量转发使用的标签。  

* 单播MAC地址的传播过程  

    如图所示，单播MAC地址的传播过程如下：  
    1. Site1通过ARP请求报文或ARP免费报文将自己站点内的MAC A地址及其对应的IP地址通告至Site2，ARP请求报文或ARP免费报文在经过PE1时，PE1上会生成MAC A的MAC/IP地址通告路由；  
    2. Site2在向Site1返回ARP响应报文或者发送ARP免费报文时，PE2上同样会生成Site2内MAC地址的MAC/IP地址通告路由；  
    3. PE1和PE2之间会交换MAC/IP地址通告路由，其中携带有MAC地址信息、路由下一跳信息以及EVPN实例RT值等扩展团体属性信息；  
    4. PE1和PE2在收到对方的MAC/IP地址通告路由后，会根据RT值得匹配情况在本地找到对应的EVPN实例，然后根据MAC/IP地址通告路由的NLRI信息在该实例中构建流量转发表项，用于流量传输。  
    ![单播MAC地址传播过程](../../images/EVPN-Learning/单播MAC地址传播过程.png)  

* 单播报文传输过程  
    如下图所示，当PE学习到其他站点的MAC地址且公网隧道建立成功后，则可以向其他站点传输单播报文，其具体传输过程如下：  

    1. CE2将单播报文以二层转发的方式发送至PE2；  
    2. PE2先为单播报文封装上EVPN Label，再封装上公网LDP LSP Label，再先后封装PE2的MAC地址和PE1的MAC地址。然后将封装后的单播报文发送至PE1；  
    3. PE1收到封装后的单播报文后，进行解封装，并根据EVPN Label将单播报文发送至对应EVPN站点。  
    ![单播报文传输过程](../../images/EVPN-Learning/单播报文传输过程.png)  

* 多播报文传输过程  
    当PE设备间的邻居关系建立成功后，PE设备间会互相发送集成多播路由。根据集成多播路由中的RT值，PE上的EVPN实例可以感知到与自己属于同一个EVPN的EVPN实例所在的PE，即到达这些PE的可达性信息。在PE获取可达性信息并且成功建立MPLS隧道之后，即可进行多播报文传输。如下图所示，传输过程如下：  
    1. CE1将多播报文发送至PE1；
    2. 由PE1向属于同一EVPN的远端PE2、PE3逐个发送多播报文。即PE1将多播报文复制成两份，每份报文将先后封装上EVPN BUM Label、公网隧道标签，再先后封装PE1的MAC地址和P的MAC地址，然后发送给远端PE；  
    3. PE2和PE3收到多播报文后，将对报文解封装并根据EVPN BUM   

    Label将多播报文发送至对应EVPN的站点。  
    ![多播报文传输过程](../../images/EVPN-Learning/多播报文传输过程.png)  
    在CE双归PE的组网中，两个PE间发送多播报文时，根据水平分割原理，多播报文还会封装上EVPN ESI Label，防止传输多播报文过程中出现环路。如下图所示，CE1双归接入PE1和PE2。当PE1收到CE1发送来的多播报文时，封装ESI之后再发送给PE2；当PE2收到该多播报文后，发现携带的ESI与本地ESI相同，PE2丢弃该报文，从而避免环路。  
    ![ESI防止环路](../../images/EVPN-Learning/ESI防止环路.png)  

#### EVPN-MPLS

1. EVPN多归技术  

    + 多归技术相关概念   

        * DF选举  
        如下图所示，CE1双归属至PE1和PE2,CE2发送多播流量至PE1和PE2,为了避免CE1从PE1和PE2收到重复的流量而造成的网络资源浪费，EVPN引入了DF选举机制，即从PE1和PE2中指定一个PE来转发多播流量。如果PE1被选举为主DF，则从CE2方向发来的多播流量只从PE1向CE1转发，而未被选中的PE2则成为备份DF。  
        如果PE与CE连接的接口状态为Down，则该PE成为备份DF。如果PE与CE连接的接口状态为UP，则该PE与其他接口同样为UP的PE共同选举出一个主DF，具体选举过程如下：  

            1. 各个PE设备之间建立邻居关系后互相发送以太网段路由;  
            2. 根据以外网段路由中携带的ESI值，各个PE上会生成多归PE列表，多归PE列表中包含连接到同一个CE的所有PE的信息;  
            3. 通过从其他PE收到的以太网段路由获取Source IP地址，根据Source IP地址大小的顺序对多归PE列表内的PE进行排序，并且顺序分配由0开始的序号;  
            4. 如果是基于接口的DF选举，则Source IP地址小的PE被选为主DF;如果是基于VLAN进行DF选举，则需要按照公式:(V mod N)=i计算出作为DF的PE设备的序号，其中i表示PE的序号，N为多归到同一CE的PE的数量，V表示Ethernet Segment对应VLAN的VLAN ID。  

        ![DF选举功能示意图.png](../../images/EVPN-Learning/DF选举功能示意图.png)  

        * 水平分割  
        如下图所示，CE1双归属至PE1和PE2且使能了负载分担时，如果PE1和PE2之间建立了邻居关系，则当PE1从CE1收到了多播流量后，PE1会将多播流量转发至PE2。为了避免PE2继续将流量转发至CE1形成环路，EVPN中定义了水平分割功能，即在PE1收到来自CE1的多播流量会转发给PE2,PE2收到报文后检查流量中携带的EVPN ESI Label，发现该标签中的ESI值等于PE2与CE1连接的网段的ESI，则PE2不会将该多播流量发送至CE1,从而避免形成环路。  
        ![水平分割功能示意图.png](../../images/EVPN-Learning/水平分割功能示意图.png)  

        * 冗余模式与别名  
        在CE多归属组网中，可以配置EVPN组网的冗余模式，即配置与同一CE相连的各个PE的活跃模式为多活跃模式或单活跃模式。通过配置冗余模式可以控制CE多归属组网是否对单播流量进行负载分担。如下图所示，如果PE1和PE2都配置为多活跃模式，在PE1和PE2通过以太网自动发现路由将冗余模式的信息发送给PE3后，则PE3在向CE1发送单播流量时，会以负载分担的形式同时向PE1和PE2发送流量。  
        此外EVPN还支持别名功能。在CE多归多活场景中，多归的PE中有些PE可能学习不到CE侧的MAC地址，此时远端PE可以通过多归PE发来的以太网自动发现路由携带的ESI值来感知到CE侧的MAC地址可达性，这个功能被称为别名。如下图所示，PE1和PE2中仅有PE1向PE3发送了携带CE1侧MAC地址的MAC/IP地址通告路由，但是PE3可以通过以太自动发现路由感知到PE2也可以到达CE1,既可以形成负载分担。  
        ![冗余模式与别名示意图.png](../../images/EVPN-Learning/冗余模式与别名示意图.png)  

        * 快速收敛  
        如下图所示，当CE1和PE1之间的链路出现故障时，PE1会对PE3发布撤销类型的以太自动发现路由，即向PE3通告其对Site1可达性变成了不可达。当PE3收到以太自动发现路由后，PE3将仅使用PE2向Site1发送流量，这样可以避免逐条发送MAC路由撤销信息，大大减少了收敛时间。  
        ![快速收敛功能示意图.png](../../images/EVPN-Learning/快速收敛功能示意图.png)  
2. EVPN Seamless MPLS的基本原理  
3. EVPN的服务模式  

#### EVPN-VXLAN

* EVPN-VXLAN基本原理  

#### EVPN-VPWS

* EVPN-VPWS基本原理  

#### PBB-EVPN

* PBB-EVPN基本原理  
* 部署H-VPLS网络迁移到PBB-EVPN网络  

#### EVPN E-Tree

#### EVPN MAC迁移抑制
在EVPN ELAN网络中，可能由于部署原因导致两个PE设备之间可以同时通过网络侧和接入侧链路互连。这样该组网中会出现BUM流量环路，并发生MAC路由震荡现象，从而影响设备正常使用。系统中的EVPN MAC迁移抑制功能可以解决这个问题。  
![EVPN的BUM流量环路示意图.png](../../images/EVPN-Learning/EVPN的BUM流量环路示意图.png)  
如上图所示，PE1和PE2之间部署EVPN协议。CE1和CE2可以采用VLAN、QinQ、PW或静态VXLAN几种方式中的一种来接入PE1和PE2。PE1和PE2可以同时通过网络侧和接入侧链路互连，这样将产生BUM流量环路：  

1. PE1从CE1收到BUM流量后，PE1将同时向网络侧和接入侧链路复制转发流量(图中流量1)。  
2. PE2从网络侧收到PE1发送的BUM流量后，PE2会往接入侧链路继续转发(图中流量2)，同时PE2也会将从接入侧收到的BUM流量向网络侧转发(图中流量3)。  
3. PE1从网络侧收到PE2发送的BUM流量后，PE1会往接入侧链路继续转发(图中流量4)，同时PE1也会将从接入侧收到的BUM流量向网络侧转发(图中流量5)。  
4. 重复步骤2和3，BUM流量将在PE1和PE2之间不断循环。  

![EVPN的路由振荡示意图.png](../../images/EVPN-Learning/EVPN的路由振荡示意图.png)  
如图所示，在发生BUM流量环路的同时，网络中还会发生路由振荡：  

1. PE1从CE1收到BUM流量后，PE1将从流量的源MAC地址学习到CE1的MAC地址M1。PE1通过EVPN协议向PE2发送目的地址为M1的MAC路由。  
2. PE2收到M1的MAC路由并匹配RT后，该MAC路由将交叉到相应EVPN实例中，并生成MAC表项，同时迭代到通往PE1的网络侧VXLAN/MPLS隧道。  
3. PE2还可以从与PE1直连的接入侧链路收到BUM流量。在收到该流量后，PE2也会根据流量的源MAC地址生成M1的MAC路由。此时PE2将认为M1已经迁移到了自己的接入侧网络。PE2会优选本地接入侧收到的MAC，所以PE2将向PE1发送M1的MAC路由，同时携带MAC迁移属性，迁移序列号为seq0+1。  
4. PE1从PE2收到M1的MAC路由并匹配RT后，PE1会将该MAC路由交叉到相应的EVPN实例中。由于迁移序列号大的路由优先，所以PE1优选从PE2收到的MAC路由并生成MAC表项，同时迭代通往PE2的网络侧VXLAN/MPLS隧道。然后PE1会向PE2发送MAC路由撤销信息。  
5. PE1再次从接入侧链路收到BUM流量后，PE1会再次生成M1的MAC路由，并认为M1已经迁移到了自己的接入侧网络。PE1会优选本地MAC路由并向PE2发送M1的MAC路由，该路由将会携带MAC迁移属性迁移序列号为seq0+2。  
6. PE2从PE1收到M1的MAC路由并匹配RT后，PE1会将该MAC路由交叉到相应的EVPN实例中。由于迁移序列号大的路由优先，所以PE2优先从PE1收到的MAC路由并生成MAC表项，同时迭代通往PE1的网络侧VXLAN/MPLS隧道。然后PE2会向PE1发送MAC路由撤销信息。  
7. PE2再次从与PE1直连的接入侧链路收到BUM流量后，PE2会再次生成M1的MAC路由，并认为M1再次迁移到了自己的接入侧网络。PE2会优选本地MAC路由并向PE1发送M1的MAC路由，该路由会携带MAC迁移属性，迁移序列号为seq0+3。  
8. 如上所述，重复步骤3至步骤7,MAC路由的迁移序列号将不断加1,网络中形成路由振荡。  

为了避免此种情况下产生的流量环路和路由振荡，系统会启动MAC迁移抑制功能。系统会检查一个检测周期内MAC表项跳变的次数，如果跳变次数超过阈值，则系统会认为网络中发生了MAC路由振荡，并对振荡的MAC路由进行抑制，即不能再通过BGP EVPN邻居关系发送给远端PE设备。  
除了对振荡的MAC路由进行抑制之外，用户还可以通过配置实现黑洞MAC路由与接入侧接口阻断功能：  

* 部署黑洞MAC路由功能后，系统会将抑制的MAC路由设置为黑洞路由。如果PE设备再次收到源MAC地址或目的MAC地址与黑洞MAC路由的MAC地址相同的转发流量，则PE会将该流量丢弃。  
* 如果用户同时还部署了接入侧接口阻断功能，即如果流量来自本地的一个接入侧接口，且该流量的源MAC地址为黑洞MAC路由的MAC地址，那么该接入侧接口将会被阻断，这样可以更快速的破环。仅BD模式的EVPN实例支持接入侧接口的阻断功能。  
