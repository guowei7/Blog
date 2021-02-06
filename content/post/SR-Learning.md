---
title: "SR Learning"
date: 2021-02-05T10:29:13+08:00
Description: "记录SR的学习"
Tags: ["Segment Route"]
Categories: ["计算机网络"]
DisableComments: false
---

主要是对Segment Route学习的记录
<!--more-->

### 1. Segment Routing MPLS  

#### 1.1 Segment Routing MPLS 特性描述  

1. Segment Routing MPLS 介绍  

+ 定义  
    段路由SR(Segment Routing)是基于源路由理念而设计的在网络上转发数据包的一种协议。Segment Routing MPLS是指基于MPLS转发平面的Segment Routing，下文简称Segment Routing。Segment Routing将网络路径分成一个个段，并且为这些段和网络中的转发节点分配段标识ID。通过对段和网络接点进行有序排列(Segment List)，就可以得到一条转发路径。  
    Segment Routing将代表转发路径的段序列编码在数据包头部，随数据包传输。接收端收到数据包后，对段序列进行解析，如果段序列的顶部段标识是本节点时，则弹出该标识，然后进行下一步处理；如果不是本节点，则使用ECMP(Equal Cost Multiple Path)方式将数据包转发到下一节点。  

+ 目的  
    随着时代的进步，网络业务种类越来越多，不同类型业务对网络的要求不尽相同，例如，实时的电话会议、视频直播等应用程序通常更喜欢低时延，低抖动的路径，而大数据应用则更喜欢低丢包率的高带宽通道。如果仍旧按照网络适配业务的思路，则不仅无法匹配业务的快速发展，而且会使网络部署越来越复杂，变的难以维护。  
    解决思路就是业务驱动网络，由业务来定义网络的架构。具体来说，就是由应用提出需求(时延、带宽、丢包率等)，控制器收集网络拓扑、带宽利用率、时延等信息，根据业务需求计算显示路径。 
    Segment Routing正是在此背景下产生的。通过Segment Routing可以简易的定义一条显示路径，网络中的节点只需要维护Segment Routing信息，即可应对业务的实时快速发展。Segment Routing具有如下特点：  
    - 通过对现有协议(IGP)进行扩展，能使现有网络更好的平滑演进。  
    - 同时支持控制器的集中控制模式和转发器的分布控制模式，提供集中控制和分布控制之间的平衡。  
    - 采用源路由技术，提供网络和上层应用快速交互的能力。  

+ 受益  
    使用Segment Routing技术，将带来明显的受益：  
    - 简化MPLS网络的控制平面。  
        Segment Routing使用控制器或者IGP集中算路和分发标签，不再需要RSVP-TE，LDP等隧道协议。Segment Routing可以直接应用于MPLS架构，转发平面没有变化。  

    - 提供高效TI-LFA(Topology-Independent Loop-free Alternate)FRR保护，实现路径故障的快速恢复。  
        在Segment Routing技术的基础上结合RLFA(Remote Loop-free Alternate)FRR算法，形成高效的TI-LFA FRR算法。TI-LFA FRR支持任意拓扑的节点和链路保护，能够弥补传统隧道保护技术的不足。  

    - Segment Routing技术更具网络容量扩展能力。  
        传统MPLS TE是一种面向连接的技术，为了维护连接状态，节点间需要发送和处理大量Keepalive报文，设备控制层面压力大。Segment Routing仅在头节点对报文进行标签操作即可任意控制业务路径，中间节点不需要维护路径信息，设备控制层面压力小。  
        此外，Segment Routing技术的标签数量是：全网节点数+本地领接数，只和网络规模相关，与隧道数量和业务规模无关。  

    - 更好地向SDN网络平滑演进。  
        Segment Routing技术基于源路由理念而设计，通过源节点即可控制数据包在网络中的转发路径。配合集中算路模块，即可灵活简便的实现路径控制与调整。  
        Segment Routing同时支持传统网络和SDN网络，兼容现有设备，保障现有网络平滑演进到SDN网络，而不是颠覆现有网络。  

2. Segment Routing MPLS 原理描述  

    + Segment Routing MPLS 基本原理  
        - 基本概念  
            Segment Routing MPLS特性涉及的概念如下：
            1. SR域(Segment Routing Domain):SR节点的集合。  
            2. SID：即Segment ID，用来标识唯一的段。在转发层面，可以映射为MPLS标签。  
            3. SRGB(Segment Routing Global Block):用户指定的为Segment Routing MPLS预留的全局标签集合。  
            4. SRLB(Segment Routing Local Block):用户指定的为Segment Routing MPLS预留的本地标签集合。这些标签在本地配置，仅在本地有效，但是会通过IGP对外发布，所以是全局可见。SRLB当前主要用于配置Binding SID。  

        - Segment 分类  
            |标签|生成方式|作用|
            |:--:|:--:|:--:|
            |prefix Segment(前缀段)|手工配置|Prefix Segment用于标识网络中的某个目的地址前缀。通过IGP协议扩散到其他网元，全局可见，全局有效。通过SID标识。Prefix SID是源端发布的SRGB范围内的偏移值，接收端会根据自己的SRGB计算实际标签值用于生成MPLS转发表项。|
            |Adjacency Segment(邻接段)|源节点通过协议动态分配。也可以手工配置|用于表示网络中的某个邻接。通过IGP协议扩散到其他网元，全局可见，本地有效。通过Adjacency SDI标识。为SRGB范围外的本地SID。|
            |Node Segment(节点段)|手工配置|Node Segment是特殊的Prefix Segment，用于标识特定的节点(Node)。在节点的Loopback接口下配置IP地址作为前缀，这个节点的Prefix SID实际就是Node SID。|  
            通俗的理解，Prefix SID代表目的地址，Adjacency SID代表数据包的外发链路，可以分别类似于传统IP转发中的目的IP地址和出接口。在IGP区域内，网元设备使用扩展IGP消息将自身的Node SID以及Adjacency SID进行泛洪，这样任意一个网元都可以获得其他网元的信息。  
            通过按序组合前缀(节点)SID和邻接SID，可以构建出网络内的任何路径。在路径中的每一跳，使用栈顶段信息区分下一跳。段信息按照顺序堆叠在数据头的顶部。当栈顶段信息包含另一个节点的标识时，接收节点使用等价多路径(ECMP)将数据包转发到下一跳。当栈顶信息是本节点的标识时，接收节点弹出顶部段并执行下一个段所需的任务。  
            实际应用中Prefix SID、Adjacency SID、Node SID可以单独使用，也可以结合使用。主要有如下三种情况。  
            1. Prefix SID  
                基于Prefix SID的转发路径是由IGP通过最短路径算法(SPF)计算得出。以节点Z为目的节点，其Prefix SID是100,通过IGP扩散后，整个IGP域的所有设备学习到节点Z的prefix SID，之后都会使用SPF算法得出一条到节点Z的最短路径，也即开销最小路径。  
                
            2. 
        - 
    + IS-IS for SR-MPLS
    + OSPF for SR-MPLS
    + BGP for SR-MPLS
    + SR-MPLS BE
    + SR-MPLS TE
    + 跨域E2E SR-MPLS TE
    + 流量导入
    + SBFD for SR-MPLS
    + SR-MPLS TE Policy
    + TI-LFA FRR
    + Anycast FRR
    + SR-MPLS防微环  
    + SR-MPLS OAM
    + MPLS in UDP
    + SR-MPLS TTL  

3. Segment Routing MPLS 应用  
4. Segment Routing MPLS 术语与缩略语

#### 1.2 Segment Routing MPLS配置

### 2. Segment Routing IPv6  

#### 2.1 Segment Routing IPv6特性描述  

#### 2.2 Segment Routing IPv6配置
