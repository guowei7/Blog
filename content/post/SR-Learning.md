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
        此外，Segment Routing技术的标签数量是：全网节点数+本地邻接数，只和网络规模相关，与隧道数量和业务规模无关。  

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
                如果网络中存在等价路径，则可以实现负载分担;如果存在不等价路径，则可以形成链路备份。由此可见，基于Prefix SID的转发路径并不是一条固定路径，头节点也无法控制报文的整条转发路径。  

            2. Adjacency SID  
                通过给网络中没个邻接分配一个Adjacency SID，然后在头节点定义一个包含多个Adjacency SID的Segment List，就可以严格指定任意一条显式路径(Strict Explicit)。这种方式可以更好的配合实现SDN。  

            3. Adjacency SID + Node SID
                这种方式是将Adjacency SID和Node SID结合，通过Adjacency SID，可以强制整条路径包含某一邻接。而对于Node SID，节点可以使用SPF算法计算最短路径，也可以负载分担。这种方式的路径并不是严格固定，所以也称作松散路径(Loose Explicit)。  

        - Segment Routing 转发机制  
            Segment Routing可以直接应用在MPLS架构，转发机制没有变化。代表段的SID被编码为MPLS标签。段序列被编码为标签栈。要处理的段位于栈顶。一个段处理完成后，相关标签从标签栈中弹出。  
            基于MPLS转发机制的SR，通常也称为SR-MPLS。  

        - Segment Routing 标签冲突处理原则  
            由于Prefix SID通过手工配置生成，不同设备上的配置可能发生标签冲突。标签冲突分为前缀冲突和SID冲突，前缀冲突是指相同的前缀关联了两个不同的SID，SID冲突是指相同的SID关联到不同的前缀。  
            标签冲突处理原则：当冲突产生后，优先处理前缀冲突，之后根据处理结果再进行SID冲突处理，并按如下规则进行优选。  

            1. 前缀掩码更大者优选； 
            2. 前缀更小者优选； 
            3. SID更小者优选。  
            
            假如现在有如下四条路由(前缀/掩码 SID):  
            
            * a. 1.1.1.1/32 1  
            * b. 1.1.1.1/32 2  
            * c. 2.2.2.2/32 3  
            * d. 3.3.3.3/32 1  
            
            使用冲突处理原则后效果如下：  

            1. 先进行前缀冲突处理，a和b为前缀冲突，根据标签冲突处理原则，a的SID比b的SID小，优选a，则处理后：  
            
                * a. 1.1.1.1/32 1  
                * c. 2.2.2.2/32 3  
                * d. 3.3.3.3/32 1  
            
            2. 再根据上一步处理结果进行SID冲突处理，a和d为SID冲突，根据标签冲突处理原则，a的前缀比d的前缀小，优选a。冲突解决后，最终优选出两条路由：  
            
                * a. 1.1.1.1/32 1  
                * c. 2.2.2.2/32 3  

    + IS-IS for SR-MPLS  
        SR-MPLS使用IGP协议进行拓扑信息、前缀信息、SRGB和标签信息的通告。IGP协议为了完成上述功能，对于协议报文的TLV进行了一些扩展。IS-IS协议住要定义了针对SID和网元SR-MPLS能力的子TLV。如下表：  

        |名称|作用|携带位置|  
        |:--:|:--:|:--:|  
        |Prefix-SID Sub-TLV|用于通告SR-MPLS的Prefix SID|IS-IS Extended IPv4 Reachability TLV-135等|  
        |Adj-SID Sub-TLV|用于在p2p网络中通告SR-MPLS的Adjacency SID|IS-IS Extended IS reachability TLV-22|  
        |LAN-Adj-SID Sub-TLV|用于在LAN网络中通告SR-MPLS的Adjacency SID|  
        |SID/Label Sub-TLV|用于通告SR-MPLS的SID或MPLS Label|SR-Capabilities Sub-TLV和SR Local Block Sub-TLV|  
        |SID/Label Binding TLV|用于通告前缀到SID的映射|IS-IS LSP|  
        |SR-Capabilities Sub-TLV|用于对外通告自己的SR-MPLS能力|IS-IS Router Capability TLV-242|  
        |SR-Algorithm Sub-TLV|用于对外通告自己使用的算法|IS-IS Router Capability TLV-242|  
        |SR Local Block Sub-TLV|用于通告网元为本地SID预留的标签范围|IS-IS Router Capability TLV-242|  

        - IS-IS对于SID的TLV扩展  
            * Prefix-SID Sub-TLV  
                Prefix-SID sub-TLV携带IGP-Prefix-SID信息，格式如下：  
                
                >Type(3)-Length-Flags-Algorithm  
                >SID/Index/Label(variable)  

            * Adj-SID Sub-TLV  
                Adj-SID Sub-TLV是一个可选子TLV，携带了IGP Adjacency SID信息，格式如下：  

                >Type(31)-Length-Flags-Weight  
                >SID/Label/Index(variable)  

                DIS(Designated Intermediate System,指定中间系统)是IS-IS在LAN网络通信过程中，被选举出来作为中介的一个中间系统。在LAN网络中，网元仅需要发布一条到DIS的链路信息，然后从DIS获取全部的链路信息，网元间不需要相互交换链路信息。  
                在Segment Routing实现中，每一个网元需要发布到所有邻居的Adj-SID。在LAN网络中，每个网元仅发布一条到DIS的IS-IS Extended IS reachability TLV-22，然后把到其他邻居的Adj-SID封装在一个新的TLV中，这就是LAN-Adj-SID Sub-TLV。该TLV包含了网元为所有LAN网络邻居分配的Adj-SID。  

                >Type-Length-Flags-Weight  
                >System-ID(6 octets)
                >SID/Label/Index(variable)  

            * SID/Label Sub-TLV  
                SID/Label Sub-TLV包含一个SID或MPLS Label。SID/Label Sub-TLV是SR-Capabilities Sub-TLV的一部分。  

                >Type(1)-Length  
                >SID/Label(variable)  

            * SID/Label Binding TLV  
                SID/Label Binding TLV用在SR与LDP互通场景，SID/Label Binding TLV可以通过前缀到SID的映射关系。格式如下：  

                >Type(1)-Length-Flags-Reserved  
                >Range-Prefix Length-Prefix  
                >Prefix(Continued,variable)  
                >SubTLVs(variable)  

        - IS-IS对于能力的TLV扩展  
            * SR-Capabilities Sub-TLV  
                Segment Routing要求各个网元都通告自己的SR能力和全局SID范围(或者全局标签索引值)，为了实现上述要求，新定义SR-Capabilities Sub-TLV，并且通过嵌入IS-IS Router Capability TLV-242进行传递。SR-Capabilities Sub-TLV只能在同一个IS-IS级别里传播，不能传播到该级别区域之外。  

                >Type(2)-Length-Flags  
                >Range  
                >SID/Label Sub-TLV(variable)  

            * SR-Algorithm Sub-TLV  
                网元可能使用不同的算法来计算到其他节点/前缀的可达信息，比如最短路径优先SPF算法，以及各种SPF的变种算法等。通过新定义的SR-Algorithm Sub-TLV，网元可以对外通告自己使用的算法，SR-Algorithm Sub-TLV也通过嵌入IS-IS Router Capability TLV-242进行传递。SR-Algorithm Sub-TLV只能在同一个IS-IS级别里传播，不能传播到该级别区域之外。  

                >Type(2)-Length  
                >Algorithm1-Algorithm2-Algorithm...-Algorithm n  

            * SR Local Block Sub-TLV  
                SR Local Block Sub-TLV中包含了网元为本地SID预留的标签范围。本地SID用于Adjacency SID，也可能是由其他组件分配，比如，其他应用或控制器可能会指定网元分配一个特殊的本地SID。因此，为了让APP或控制器知道哪些本地SID在本网元可用，网元有必要对外通告SRLB。  

                >Type-Length-Flags  
                >Range  
                >SID/Label Sub-TLV(variabel) 一个或多个  

                网元通告的SRLB TLV也可能有其他标签范围，不再SRLB范围内，可能是用于本地分配，所以不再SRLB中通告。例如，有可能Adjacency SID被分配了一个本地标签，而不是SRLB范围内的标签。  

    + OSPF for SR-MPLS  
    + BGP for SR-MPLS  
        IGP for SR-MPLS只能在自治系统AS域内分配SID，通过对AS域内SID的合理编排，可以规划出AS域内的最优路径。为了组建大规模网络，通常需要跨越多个AS，此时IGP for SR-MPLS无能为力。BGP for SR-MPLS是BGP针对Segment Routing的扩展，能够针对BGP的Peer相关信息分配BGP Peer SID，并将该信息上报给控制器。之后SR-MPLS TE在编排路径时，使用BGP Peer SID作为编排路径中的一环，从而得到跨域E2E SR-MPLS TE最优路径。BGP for SR-MPLS主要包括BGP EPE(Egress Peer Engineering)扩展和BGP-LS扩展。  

        * BGP EPE  
            BGP EPE可为域间路径分配BGP Peer SID，Peer SID可以通过BGP-LS扩展直接传递给网络控制器。而对于没有与控制器建立BGP-LS邻居的转发器设备，可以先通过BGP-LS将Peer SID信息传递给BGP邻居，然后再由BGP邻居通过BGP-LS传递给网络控制器。  
            BGP EPE能够针对对等体分配Peer Node Segment(Peer-Node SID)、Peer Adjacency Segment(Peer-Adj SID)和Peer-Set SID。  

            1. Peer-Node SID用于指示一个对等体节点。每一个BGP会话都会分配Peer-Node SID。对于基于Loopback接口建立的EBGP邻居，其对应的物理链路可能有多条，那么针对该邻居的Peer-Node SID就会对应多个出接口。  
            2. Peer-Adj SID用于指示到达对等体的一个邻接。对于基于Loopback接口建立的EBGP邻居，其对应的物理链路可能有多条，则每个邻接都会分配一个Peer-Adj SID，转发时只能使用指定的链路进行转发。  
            3. Peer-Set SID用于指示一组邻居。BGP支持将一组邻居规划为一个Set，然后基于该组分配SID，这个SID称为Peer-Set SID，Peer-Set SID在转发时可以对应多个出接口。一个Peer-Set由众多Peer-Node和Peer-Adj组成，那么针对该Peer-Set分配的SID就会对应到多个Peer-Node SID和Peer-Adj SID。  

            Peer-Node SID和Peer-Adj SID均是局部标签，在本机有效。不同设备的Peer-Node SID和Peer-Adj SID可以重复。当前BGP EPE仅支持EBGP邻居场景，对于多跳EBGP邻居，也必须是通过物理链路直连。  
            
            BGP EPE只是针对BGP邻居节点及相关链路分配SID，并不能构建一个用于转发的隧道，BGP Peer SID信息还需要IGP SID配合才能形成E2E的隧道。当前的IPv4 SR主要有SR LSP(SR-MPLS BE)和SR-MPLS TE两种方式。  

            1. SR LSP是转发器直接基于域内IGP SID信息进行动态计算得到的，而BGP EPE分配的Peer SID信息并不能被IGP使用，因此当前无法支持跨域SR LSP。  
            2. SR-MPLS TE可以通过静态指定路径的方式来建立一条E2E隧道，也可以由网络控制器编排，只要指定的路径里包含AS域间的链路信息即可。  

            故障联动  
            域内配置SR-MPLS TE隧道，并且配置SBFD for SR-MPLS TE隧道。域间配置BGP EPE功能。这样可以建立两条跨域SR-MPLS TE隧道，隧道由域内SID和域间BGP SID编排而成。如果域间BGP EPE对应的链路发生故障，SR-MPLS TE隧道头节点无法感知，可能产生流量黑洞。有两种方式解决上述问题：  

            1. 当域间链路故障时，BGP EPE联动本AS域内SBFD for SR-MPLS TE隧道状态置Down，从而使SR-MPLS TE隧道头节点快速感知故障，将数据流量切换到其他正常路径转发。  
            2. 当域间链路故障时，节点将SR报文的BGP EPE标签剥掉(Pop)，利用报文的目的地址查找IP路由表转发，这样报文可能从别的IP链路绕行到达另一个域。这种方式适用于网络里不存在备份跨域SR-MPLS TE隧道场景。  

        * BGP-LS  
            跨域E2E SR-MPLS TE隧道可以通过静态显式路径建立，也可以通过控制器编排。其中控制器编排场景，域内SID和域间SID都通过BGP-LS上报给控制器，同时域间链路也要支持配置TE链路属性并上报给控制器。控制器基于SR-MPLS TE隧道的属性来计算主备路径。其中，对于BGP EPE发现的网络拓扑和分配的标签信息，BGP-LS是打包在Link NLRI信息中发布给控制器。格式如下：  
            ***Peer-Node SID***  
            NLRI(LocalDescriptor,RemoteDescriptor,LinkDescriptor)+LinkAttribute(Peer-Node SID)  
            ***Peer-Adj SID***  
            NLRI+LinkAttribute(Peer-Adj SID,Administrative Group,Max Link BW,Max Reservable Link BW,Unreserved BW,Shared Risk Link Group)  
            ***Peer-Set SID***  
            NLRI+LinkAttribute(Peer-Node SID,Peer-Set SID)  
            NLRI+LinkAttribute(Peer-Adj SID,Peer-Set SID,Administrative Group,Max Link BW,Max Reservable Link BW，Unreserved BW,Shared Risk Link Group) 

            |字段|解释|
            |:--:|:--:|
            |NLRI|网络层可达信息(Network Layer Reachability Information)。LocalDescriptor:本地描述，由本机Router ID，本机AS号和BGP-LS ID组成。RemoteDescriptor:远端描述，由邻居Router ID和邻居AS号组成。LinkDescriptor：链路描述，包含BGP会话使用的地址。|
            |LinkAttribute|链路信息。是Link NLRI的一部分。Peer-Node SID：邻居节点SID。这里代表Peer-Node SID TLV。Peer-Adj SID：到达邻居的邻接SID。这里代表Peer-Adj SID TLV。Peer-Set SID：到达邻居组的SID。Administrative Group：链路管理组属性。Max Link BW：最大链路带宽。Max Reservable Link BW：最大预留链路带宽。Unreserved BW：剩余链路带宽。Shared Risk Link Group：共享风险链路组SRLG|  

            Peer-Node SID TLV和Peer-Adj SID TLV具有相同的格式。如下：
            
            >Type(2 octets)+Length(2 octets)  
            >Flags(1 octet)+Weight(1 octet)+Reserved(2 octets)
            >SID/Label/Index(variable) 

    + SR-MPLS BE  
        SR LSP是指使用Segment Routing技术建立的标签转发路径，由一个Prefix SID或Node SID指导数据包转发。SR-MPLS BE（Segment Routing-MPLS Best Effort)是指IGP使用最短路径算法计算得到的最优SR LSP。  
        SR LSP的创建过程和数据转发与LDP LSP类似。这种LSP不存在Tunnel接口。  

        - SR LSP创建  
            SR LSP创建需要完成以下动作：  

            * 网络拓扑上报(仅在基于控制器创建LSP时需要)/标签分配。  
            * 路径计算。  

            对于SR LSP，主要基于前缀标签创建。目的节点通过IGP协议发布Prefix SID，转发器解析Prefix SID并根据自己的SRGB计算标签值。此后各节点使用IGP协议收集的拓扑信息，根据最短路径算法计算标签转发路径，并将计算的下一跳及出标签(OuterLabel)信息下发转发表，指导数据报文转发。  

            LSP创建过程如下：  

            |步骤|设备|操作|
            |:--:|:--:|:--:|
            |1|D|在D上配置SRGB，在D的Loopback口配置Prefix SID，生成转发表项并下发。之后D将SRGB和Prefix SID封装到LSP报文，并将LSP报文通过IGP向全网扩散。网络中其他设备接收到LSP报文后，解析D发布的Prefix SID，根据自己的SRGB计算标签值，同时根据下一跳节点发布的SRGB计算出标签值(OuterLabel)。使用IGP拓扑计算标签转发路径，然后生成转发表项。|
            |2|C|解析D发布的Prefix SID，根据自己的SRGB=[36000-65535]计算标签值，计算公式是：Label=SRGB的起始值+Prefix SID值。使用IS-IS拓扑计算出标签(OuterLabel)，计算公式是：OuterLabel=下一跳设备发布的SRGB的起始值+Prefix SID值，在本例中，下一跳设备为D，D发布的SRGB范围是[16000-65535]。|
            |3|B|计算过程与C类似|
            |4|A|计算过程与C类似|  

            跨IGP域建立SR LSP  
            以IS-IS为例，由于IS-IS的报文泛洪范围是区域内，为了跨区域建立Segment Routing LSP，需要将前缀SID通过渗透跨区域传播。  
            A和D部署在不同区域中，所有设备采用IS-IS互连，同时部署Segment Routing。现在需要建立从A到D的SR LSP。  
            在D上配置SRGB，在D的Loopback口配置Prefix SID，D生成并下发转发表项，D将SRGB和Prefix SID封装到LSP报文;C将解析Prefix SID，计算转发表项并下发，同时将Prefix SID连同Prefix渗透到Level-2;B解析Prefix SID，计算转发表项并下发，同时将Prefix SID连同Prefix渗透到Level-1;A解析Prefix SID，使用IS-IS拓扑，采用Dijkstra算法，计算标签转发路径;生成LSP转发表项。  

        - 数据转发  
            SR的标签操作类型和MPLS相同，包括标签栈压入(Push)、标签栈交换(Swap)和标签弹出(Pop)。  

            * Push:当报文进入SR LSP时，入节点设备在报文二层首部和IP首部之间插入一个标签;或者根据需要，在报文标签栈的栈顶增加一个新的标签。  
            * Swap：当报文在SR域内转发时，根据标签转发表，用下一跳分配的标签，替换SR报文的栈顶标签。  
            * Pop：当报文在离开SR域时，根据栈顶的标签查找转发出接口之后，将SR报文的栈顶的标签剥掉。  

        - PHP与MPLS QoS、TTL  
            倒数第二跳弹出PHP(Penultimate Hop Popping),是指在最后一跳节点(Egress节点),标签已经没有使用加值。这种情况下，可以利用PHP在倒数第二跳节点将标签弹出，减少最后一跳的负担。最后一跳节点直接进行IP转发或者下一层标签转发。  
            PHP在Egress节点上配置。如果没有启用PHP特性，既使网元是SR隧道的倒数第二跳，网元仍旧使用到达最后一跳网元的真实标签。如果启用了PHP特性，则网元发往最后一跳网元的报文不会携带SR标签。  
            如果启用PHP特性，则对Egress节点的MPLS QoS和TTL特性都有影响，如下表：  

            |标签方式|意义|Egress节点的MPLS EXP(QoS)|Egress节点的MPLS TTL|场景|
            |:--:|:--:|:--:|:--:|:--:|
            |显式空标签(explicit-null)|不支持PHP特性，Egress节点向倒数第二跳分配显式空标签。IPV4的显式空标签值为0|MPLS EXP字段保留，支持QoS|MPLS TTL正常|节省Egress节点的资源。当端到端业务有QoS属性，需要携带在标签EXP中时，可以使用该方式|
            |隐式空标签(implicit-null)|支持PHP特性，Egress节点向倒数第二跳分配隐式空标签。隐式空标签的值为3。当一个网元发现自己被分配了隐式空标签时，它并不用这个值替代栈顶原来的标签，而是直接执行Pop操作。Egress节点直接进行IP转发或下一层标签转发。|Egress节点的报文无MPLS EXP字段，不支持QoS。|Egress节点的报文无MPLS TTL，也不能复制到IP TTL。|减少Egress节点的转发压力，提高转发效率|
            |非空标签(non-null)|不支持PHP特性，Egress节点向倒数第二跳正常分配标签。|MPLS EXP字段保留，支持QoS。|MPLS TTL正常。|这种方式对Egress节点的资源消耗较大，不推荐使用。如果用户想要在Egress节点根据标签区分业务，可以使用该方式。|  

        - SR-MPLS BE与LDP互通  
            SR-MPLS作为一种新兴技术受到越来越多的关注，很多用户希望引入SR-MPLS技术来简化网络部署和管理，降低CAPEX(Capital Expenditure)和OPEX(Operating Expenditure)。MPLS LDP作为当前主流的隧道技术，在承载网使用广泛。这样在SR-MPLS BE逐渐替代LDP的过程中，LDP和SR-MPLS BE会在很长一段时间内共存。LDP网络和SR-MPLS BE网络之间的互通成为一个必须面对的问题。  
            SR-MPLS BE与LDP互通是一项让Segment Routing和LDP协议在同一网络中共同工作的技术。通过此技术可以让SR-MPLS BE网络连接到LDP网络中，实现两个网络之间的MPLS转发。  
            要实现LDP网络和SR-MPLS BE网络之间互通，则SR-MPLS BE网络必须有设备代替不支持Segment Routing的LDP设备发布SID，这个设备也称为Mapping Server。  

            * Mapping Server:可以配置Prefix与SID的映射关系，并且发布给Mapping Client。  
            * Mapping Client：接收Mapping Server发布的Prefix与SID的映射关系，创建映射表项。  

            由于LSP都是单向的，所以SR-MPLS BE与LDP互通包括两个关键部分：SR-MPLS to LDP和LDP to SR-MPLS。  

            * SR-MPLS to LDP  

            >PE1<-(mapping)-P1<-(mapping)-P2<-(LDP)-P3<-(LDP)-PE2  

            1. PE2上配置IP地址前缀，LDP为该前缀分配标签，并向上游P3发送标签映射消息。  
            2. P3为该前缀分配标签，并向上游P2发送标签映射消息。  
            3. P2接收到标签映射消息后，创建到PE2的LDP LSP。  
            4. P2上配置Mapping Server，将IP地址前缀携带的LDP标签映射为SID。  
            5. P2发布Mapping TLV到上游P1。  
            6. P1发布Mapping TLV到上游PE1。  
            7. PE1解析Mapping TLV，创建到P2的SR LSP。  
            8. P2上建立SR LSP与LDP LSP的映射关系。  

            在数据转发时，P2设备上没有到PE2的SR-MPLS Label，所以P2根据Prefix与SID映射关系，SR-MPLS封装映射为LDP封装。  

            * LDP to SR-MPLS  

            >PE1-(SID)-\>P1-(SID)-\>P2-(LDP)-\>P3-(LDP)-\>PE2  

            1. PE1上配置IP地址前缀，并为该前缀配置SID。然后将前缀和SID通过IGP向P1发布。  
            2. P1将前缀和SID通过IGP向P2发布。  
            3. P2接受到前缀和SID后，创建到PE1的SR LSP。  
            4. P2上配置代理LDP Egress，将IP地址前缀携带的SID映射为LDP标签。只要配置了代理LDP Egress，而且对端路由可达，就会向上游发布Mapping消息。  
            5. P2发送标签映射消息到上游P3,P3发送标签映射消息到上游PE2。  
            6. PE2解析标签映射消息，创建到P2的LDP LSP。  
            7. P2上建立SR LSP与LDP LSP的映射关系。  

            数据转发时，P2设备上没有到PE1的LDP Label，所以P2根据Prefix与SID映射关系，LDP封装映射为SR-MPLS封装。  

    + SR-MPLS TE 
        段路由流量工程SR-MPLS TE(Segment Routing-MPLS Traffic Engineering)是使用SR作为控制协议的一种新型的TE隧道技术。控制器负责计算隧道的转发路径，并将与路径严格对应的标签栈下发给转发器。在SR-MPLS TE随道的入节点上，转发器根据标签栈，即可控制报文在网络中的传输路径。  

        ***SR-MPLS TE优势***  
        随着SDN的发展，普通的RSVP-TE存在的不足已经不能满足快速发展的需求，而SR-MPLS TE作为一种新型的MPLS TE隧道技术，有着独特的优势。如下表：  

        |比较项|SR-MPLS TE|RSVP-TE|
        |:--:|:--:|:--:|
        |标签分配|通过IGP协议扩展进行标签分配和扩散。每条链路仅分配一个标签，所有的LSP共用这一个标签，减少了标签资源的占用，减轻了标签转发表的维护工作量。|通过MPLS协议扩展进行标签分配和扩散。每条LSP分配一个标签，当有多条LSP时，同一条链路上需要分配多个标签，标签资源占用的多，标签转发表的维护工作量大。|
        |控制平面|信令控制协议为IGP协议扩展，无需专门的MPLS的控制协议，减少协议数量。|需要RSVP-TE作为MPLS的控制协议，控制平面较复杂。|
        |可扩展性|网络中间设备不感知隧道，隧道信息携带在每个报文中，无需维护隧道状态信息，只需维护转发表项，可扩展性强。|需要维护隧道状态信息，也需要维护转发表项，可扩展性差。|
        |路径调整和控制|网络中间设备不感知隧道，仅通过对入节点的报文进行标签操作即可任意控制业务路径，无需逐节点下发配置。当路径中的某个节点发生故障，由控制器重新计算路径并更新入节点的标签栈，即可完成路径调整。|无论是正常业务调整还是故障场景的被动路径调整，都需逐节点下发配置。|  

        ***相关概念***  
        标签栈  
        标签栈是标签的排序集合，用于标识一条完整的标签交换路径LSP(Label Switched Path)。标签栈中每一个邻接标签标识一条具体的邻接，整个标签栈标识了整条转发路径LSP的所有邻接。在报文转发过程中，通过标签栈中每个邻接标签查找对应的邻接，并将标签弹出后转发，将标签栈中所有邻接标签弹出后，报文就走完了整条LSP，到达SR-MPLS TE隧道的目的地。  
        粘连标签与粘连节点  
        当标签栈深度超过转发器所支持的标签栈深度时，一个标签栈无法携带整条LSP的邻接标签，控制器需要为转发器分配多个标签栈，在合适的节点下发标签栈的同时分配一种特殊的标签，然后将这些标签栈关联起来，实现逐段转发。这种特殊的标签称为粘连标签，这个合适的节点称为粘连节点。  
        控制器为粘连节点分配粘连标签，并将其压在标签栈底，当报文转发至粘连节点时，根据粘连标签与标签栈的关联关系，用新的标签栈交换该粘连标签，继续指导下一段转发。  

        - 拓扑收集与标签分配  

            * 网络拓扑收集方式  
                网络拓扑收集方式主要是BGP-LS：控制器和转发器之间建立BGP-LS邻居，转发器的IGP协议收集网络拓扑信息，然后通过BGP-LS将网络拓扑信息上报给控制器。  
            * 标签分配方式  
                转发器的IGP协议分配标签，并通过BGP-LS将分配的标签上报给控制器。SR-MPLS TE主要使用邻接标签，也可以使用节点标签。邻接标签是源节点分配的，只在本地有效，并且具有一定的方向性。节点标签手工配置，全局有效。邻接标签和节点标签都可以通过IGP协议扩散出去。  

                >=======P1--P2  
                >PE1===============PE2  
                >=======P3--P4  
                
                在PE1、P1、P2、P3、P4和PE2设备上分别使能IGP SR能力，相互之间建立邻居。对于使能了SR能力的IGP实例，会对所有使能IGP协议的出接口分配SR邻接标签。邻接标签通过IGP的SR协议扩展，泛洪到整个网络中。以P3设备为例，IGP分配标签的具体过程如下：  

                1. P3的IGP协议为其邻接申请本地动态标签。例如，P3为邻接P3->P4分配的邻接标签为9002。  
                2. P3的IGP协议发布邻接标签，泛洪到整个网络。  
                3. P3上生成邻接标签对应的标签转发表。  
                4. 网络中的其它设备的IGP协议学习到P3发布的邻接标签，但是不生成标签转发表。  

                PE1、P1、P2、P3、P4按照P3的方式分配和发布邻接标签，生成标签转发表。当设备与控制器之间配置BGP-LS协议，建立了邻居关系后，BGP-LS引入带有SR标签信息的拓扑，向控制器上报。  

        - SR-MPLS TE隧道创建  

            * SR-MPLS TE隧道  
                SR-MPLS TE是指基于TE的约束属性，利用SR协议创建的隧道。  

                >=======P1--P2  
                >PE1=====Tunnel=====PE2  
                >=======P3--P4  

                有两条LSP，其中一条路径PE1->P1->P2->PE2作为主路径，另外一条PE1->P3->P4->PE2作为备份路径，而两条LSP隧道都对应同一个隧道ID的SR-MPLS TE隧道。LSP的起始节点称为入节点，位于LSP中间的节点称为中间节点，LSP的末节点称为出节点。  
                SR-MPLS TE隧道创建包括隧道配置和隧道建立。在SR-MPLS TE隧道创建之前，需要在各转发器之间建立IS-IS/OSPF邻居关系，实现网络层互通，完成标签的分配以及网络拓扑信息的收集，并将标签和网络拓扑信息上送给控制器，供控制器进行路径计算。如果没有控制器，也可以在SR-MPLS TE隧道的头节点使能CSPF算路功能，这样转发器也可以使用CSPF完成路径计算。  

            * SR-MPLS TE隧道配置  
                SR-MPLS TE隧道的属性是创建隧道的依据。SR-MPLS TE隧道可以在控制器或转发器上配置。  

                1. 控制器上配置隧道
                    在控制器上进行SR-MPLS TE隧道配置后，控制器将隧道属性通过NETCONF下发给转发器，转发器通过PCEP协议将隧道托管给控制器进行管理。  
                2. 转发器上配置随道  
                    在转发器上进行SR-MPLS TE隧道配置后，转发器将隧道托管给控制器进行管理。 

            * SR-MPLS TE隧道建立  
                当配置了某种业务(例如VPN业务)时，该业务需要绑定到SR-MPLS TE隧道，将按照以下过程建立SR-MPLS TE隧道。  

                1. 控制器基于SR-MPLS TE的隧道约束属性，通过路径计算单元PCE(Path Computation Element)进行类似普通TE的路径计算，计算出来的路径根据拓扑和邻接标签，将整条路径的邻接标签整合在一起，生成一个标签栈(即算路结果)。  
                当标签栈深度超过转发器所支持的标签栈深度时，一个标签栈无法携带所有的邻接标签，则控制器需要将整条路径分为多个标签栈。  
                2. 控制器通过NETCONF和PCEP分别将隧道配置信息和标签栈下发给转发器。  
                3. 转发器根据控制器下发的隧道配置和标签栈信息建立SR-MPLS TE随道LSP。

        - SR-MPLS TE数据转发  
            转发器上根据SR-MPLS TE随道LSP对应的标签栈，对报文进行标签操作，并根据栈顶标签逐跳查找转发出接口，指导数据报文转发到隧道目的地址。  

            * 基于邻接标签  
                通过手工指定邻接标签，设备将严格按照标签栈里指定的显式路径逐跳转发，所以这种也称作严格路径方式的SR-MPLS TE。  
            * 基于节点+邻接标签  
                严格路径方式的SR-MPLS TE在存在等价路径的情况下，也无法进行负载分担，在SR-MPLS TE路径里引入节点标签后，可以弥补上述不足。  
                手工指定节点标签+邻接标签混合标签栈，可指定跨网元的节点标签，控制器通过PCEP或NETCONF下发给转发器首节点，转发器基于该标签栈，逐跳查找出接口弹出标签，指导数据报文进行转发到隧道目的地址。  
                通过手工指定节点标签+邻接标签，设备在处理节点标签时，可以按照最短路径进行转发，也可以进行负载分担，其路径并不是严格固定，所以这种也称作松散路径方式的SR-MPLS TE。  

        - SR-MPLS TE隧道可靠性  
            SR-MPLS TE随道可靠性除了TI-LFA FRR外，还有Hot-Standby技术。  

            * SR-MPLS TE Hot-Standby  
                热备份(Hot-Standby)是指创建主LSP后随即创建备份LSP，备份LSP始终处于热备份状态。热备份LSP提供整条LSP的保护，是一种端到端的流量保护措施。  

        - BFD for SR-MPLS TE  
            SR-MPLS TE只要标签栈下发，LSP就会建立成功，且除了撤销标签栈之外，LSP不会出现协议Down的情况。所以SR-MPLS TE LSP故障检测需要依靠部署BFD检测，通过BFD故障检测切换备份LSP。BFD for SR-MPLS TE是一种端到端的快速检测机制，用于快速检测随道所经过的链路汇总所发生的故障。  

            ***BFD for SR-MPLS TE的分类***  
            BFD for SR-MPLS TE提供以下几种方式：  

            * BFD for SR-MPLS TE LSP  
                通过配置BFD for SR-MPLS TE LSP，如果主路径故障可以快速切换到备份路径。BFD for SR-MPLS TE LSP支持静态和动态的：  

                1. 静态BFD会话：本端标识符和对端标识符都需要手工指定，两端标识符必须匹配，否则会话无法建立。会话建立后，发送和接收时间间隔参数可以修改。  
                2. 动态BFD会话：不需要指定本端标识符和对端标识符。SR-MPLS TE隧道Up后，会触发BFD建立会话，会话通过协商来确定本端标识符、对端标识符、发送和接收时间间隔参数。  

                BFD会话与LSP绑定，即在入节点和出节点之间建立BFD会话。BFD报文从源端开始经过LSP转发到达宿端;宿端再对该BFD报文进行回应，通过此方式在源端可以快速检测出LSP所经过链路的状态。  
                当检测出链路故障以后，BFD将此信息上报给转发层面。转发平面查找备份LSP，然后将业务流量切换到备份LSP上。  

            * BFD for SR-MPLS TE Tunnel  
                SR-MPLS TE Tunnel状态要结合BFD for SR-MPLS TE Tunnel与BFD for SR-MPLS TE LSP检测。  

                1. BFD for SR-MPLS TE LSP用来控制主备LSP的切换状态，BFD for SR-MPLS TE Tunnel用来保证Tunnel的真实状态。  、
                    - 如果不配置BFD for SR-MPLS TE Tunnel，Tunnel默认状态只为UP，Tunnel的真实状态不确定。  
                    - 如果配置了BFD for SR-MPLS TE Tunnel，但是BFD的状态被置为管理Down，则BFD实际未工作，Tunnel接口状态是unknown。  
                    - 如果配置了BFD for SR-MPLS TE Tunnel，且BFD没有被置为管理Down，则Tunnel接口状态与BFD状态一致。  
                2. SR-MPLS TE Tunnel的接口状态依赖于BFD for SR-MPLS TE Tunnel的检测，即SR-MPLS TE Tunnel的接口状态与BFD状态一致，而BFD UP的时间由于BFD协议协商的要求会比较慢。一般情况下，当Tunnel状态为Down时下发新标签栈，到BFD UP需要十几秒的时间，这样会导致Tunnel在没有其他保护的情况下，硬收敛的时间较慢。  
            * BFD for SR-MPLS TE(单臂模式)  
                如果出节点不支持BFD for SR-MPLS TE，则无法创建BFD会话。单臂模式的BFD for SR-MPLS TE可以解决上述问题。  
                在入节点配置使能BFD检测的同时指定单臂检测模式，建立BFD会话。会话建立后，在入节点发出BFD报文，通过SR-MPLS TE Tunnel经过中间节点到达出节点后，出节点的转发平面接收到BFD报文后，将MPLS标签剥离，通过BFD报文的目的IP进行路由查找，出节点在转发平面直接将BFD报文进行环回给入节点，入节点进行检测报文处理，实现单臂环回的检测机制。  

                BFD for SR-MPLS TE正向路径沿着标签栈转发，返程走IP最短路径转发，所以返程路径不固定，而且可能由于BFD返程报文不可达造成正常隧道被BFD置Down，用户需要确保BFD返程报文的路由正常。另外，SR-MPLS TE的备份LSP无需配置BFD，主路径恢复后，隧道重新UP，路由会重新迭代到主LSP。  
            ***BFD for SR-MPLS TE的应用***
             
        - SR-MPLS TE负载分担  
            SR-MPLS TE使用头节点在数据包中封装的标签栈信息指导转发，默认情况下，每个邻接标签标识一个确定的邻接，即使网络中存在等价链路，也无法进行负载分担。为了解决上述问题，SR-MPLS TE引入了并行邻接标签，来标识网络中的多个等价链路。  
            节点B到节点E共存在三条等价链路，可以为三条链路配置相同的邻接SID，称为并行邻接标签。并行邻接标签像普通邻接标签一样参与路径计算。  
            当数据包经过节点B，且数据包使用并行邻接标签时，节点B解析并行邻接标签后，然后将数据通过HASH算法在三条等价链路上进行负载分担，从而有效利用网络资源，避免网络拥塞。  
            并行邻接标签的生成，不影响IGP邻居原有邻接标签的正常分配，即配置并行邻接标签后，设备会为同一邻接发布多个邻接标签。  
            对BFD for SR-MPLS TE，如果SR-MPLS TE路径使用了并行邻接标签，转发时同样会形成负载分担，但是每个BFD报文只会HASH到某一条链路上，如果这条链路故障，BFD可能检测到链路Down，存在误报故障风险。  

        - 根据IP报文的DSCP入SR-MPLS TE隧道  
            
            + 产生原因  
                根据IP报文的DSCP(Differentiated Service Code Point)入隧道是一种TE隧道的选择方式，与传统的TE负载分担转发相比，该方式能够基于业务的优先级选择特定的隧道进行转发，可以为高优先级的业务提供更好的服务质量。  
                当前网络中存在的主要问题是：SR-MPLS TE隧道的选择是基于公网路由或者VPN的隧道策略，对于目的地址相同的业务或者同一个VPN的业务，在选择SR-MPLS TE隧道时都会选择相同的隧道，这样无法保证高优先级的业务专享优质传输资源。  
                在支持根据IP报文的DSCP入隧道的设备上，PE节点可以根据IP报文中的DSCP值，对入口流量进行匹配，不同DCSP值的业务流量匹配到对应的隧道。CBTS(Class of service Based Tunnel Selection)只支持8个优先级：be、af1、af2、af3、af4、ef、cs6、cs7,它需要根据配置的复杂流策略将业务流量映射到对应的8个优先级上。相比于CBTS的入隧道方式，DSCP的入隧道直接根据IP业务流量中的DSCP值匹配SR-MPLS TE隧道，且支持的优先级跟加细化(0-63),用户可以更灵活的根据业务去部署SR-MPLS TE。  

            + 实现过程  
                在业务迭代到的隧道下，可以配置DCSP属性，指定隧道可以承载的一个或多个优先级。这样映射到特定优先级的业务流量就只会使用这些隧道进行转发，而不会像普通负载分担一样使用所有可能迭代到的隧道。隧道的DSCP属性也可以配置为default，用于承载剩余优先级的流量。  
                假设在PE1和PE2之间存在多条SR-MPLS TE Tunnel。当前存在三类业务：高优先级的视频业务，中优先级的语音业务和普通的以太数据业务，用户希望通过不同的SR-MPLS TE Tunnel承载这几类业务，具体的实现如下：  

                1. SR-MPLS TE Tunnel下设置DSCP属性，假设SR-MPLS TE的DSCP属性分别为15-20,5-10,default。  
                2. 在PE1上，根据视频业务流量特征(IP报文的DSCP值)，匹配到SR-MPLS TE Tunnel1;根据语音业务特征(IP报文的DSCP值),匹配到SR-MPLS TE Tunnel2;根据以太数据业务的特点(IP报文的DSCP值),匹配到DSCP属性为default的SR-MPLS TE Tunnel3承载。  

                DSCP属性为default的隧道并不是必须配置，在没有配置default的情况下，没有匹配上优先级的业务流量将由没有匹配DSCP属性的隧道进行承载，如果所有隧道都配置了DSCP属性，则会选择DSCP属性值最低的隧道进行承载。  

            + 适用场景  
                
                * PE节点场景下，公网为SR-MPLS TE Tunnel负载分担或者LDP over SR-MPLS TE，SR-MPLS TE Tunnel，非SR-MPLS TE负载分担。  
                * PE节点场景下，可以承载的业务为IP/L3VPN，包括IPv4和IPv6业务。  
                * 不支持VLL、VPLS、BGP LSP over SR-MPLS TE根据IP报文的DSCP入隧道。  
                * 不支持P节点。  

    + 跨域E2E SR-MPLS TE  

        * Binding SID  
            与RSVP-TE隧道相似，SR-MPLS TE隧道可以做为一种转发邻接。如果将SR-MPLS TE隧道做为转发邻接分配一个邻接SID，则该SID可以标识SR-MPLS TE隧道，利用这个SID就可以将数据流量导入SR-MPLS TE隧道，实施TE策略。SR-MPLS TE隧道的Adjacency SID可以称作Binding SID，Binding SID意味着将使用该SID的流量与一个SR-MPLS TE隧道或者一个TE策略绑定。  
            Binding SID在AS域内的转发器上配置，一个Binding SID代表一条域内SR-MPLS TE隧道。  
            Binding SID在AS域内的转发器上配置，一个Binding SID代表一条域内SR-MPLS TE隧道。首先在AS域内的Ingress节点配置Binding SID。  

            >======AGG1==================P1  
            >CSG1========ASBR1==ASBR3==========PE1  
            >CSG2========ASBR2==ASBR2==========PE2  
            >======AGG2==================P2  

            1. 在CSG1上配置Binding SID 6000和7000,分别代表标签栈<102,203>和<110,112,213>。  
            2. 在ASBR3上配置Binding SID 8000，代表标签栈<405,506>。  
            3. 在ASBR4上配置Binding SID 9000，代表标签栈<415,516,660>。  

            Binding SID生成以后，控制器利用Binding SID和BGP Peer SID，可以计算出E2E的跨域SR-MPLS TE隧道，用户也可以通过静态显示路径配置出跨域E2E SR-MPLS TE隧道。  
            可以看出，Binding SID是通过指定一个本地标签关联本地转发路径，用于网元转发封装。利用Binding SID不仅可以减少网元封装的标签栈层数，而且利于组建跨域E2E SR-MPLS TE网络。  

        * E2E SR-MPLS TE隧道创建  

            + E2E SR-MPLS TE隧道配置  
                E2E SR-MPLS TE隧道的属性是创建隧道的依据。E2E SR-MPLS TE隧道可以在控制器上配置，也可以在转发器上配置。  

                1. 控制器上配置隧道  
                    在控制器上进行E2E SR-MPLS TE隧道配置后，控制器将隧道属性通过NETCONF下发给转发器，转发器通过PCEP协议将隧道托管给控制器进行管理。  
                2. 转发器上配置隧道  
                    在转发器上可以严格按照显式路径指定隧道标签栈，建立手工E2E SR-MPLS TE隧道。手工E2E SR-MPLS TE隧道配置比较复杂，无法根据网络自动调整，推荐在控制器上配置隧道。  

            + E2E SR-MPLS TE隧道建立  
                当配置了某种业务(例如VPN)时，该业务需要绑定到E2E SR-MPLS TE隧道，将按照以下过程建立E2E SR-MPLS TE隧道，以下在控制器上配置隧道为例进行描述。  

                >============controller======
                >======AGG1==================P1  
                >CSG1========ASBR1==ASBR3==========PE1  
                >CSG2========ASBR2==ASBR2==========PE2  
                >======AGG2==================P2  

                隧道创建过程：  
                1. 在E2E SR-MPLS TE隧道创建之前，控制器需要先完成域内SR-MPLS TE隧道创建，并且为域内隧道配置Binding SID。然后在AS域间配置BGP EPE，生成BGP Peer SID，之后ASBR通过BGP-LS上报BGP EPE的标签和网络拓扑信息。  
                2. 控制器基于SR-MPLS TE的隧道约束属性，通过路径计算单元PCE进行类似普通TE的路径计算，计算出来的路径根据拓扑和邻接标签，将整条路径的标签整合在一起，生成一个标签栈(即算路结果)。  
                如上图所示，控制器计算出SR-MPLS TE隧道路径为CSG1->AGG1->ASBR1->ASBR3->P1->PE1，对应标签栈<6000,3040,8000>，其中6000和8000为Binding SID标签，3040为BGP Peer SID。  
                3. 控制器通过NETCONF和PCEP分别将隧道配置信息和标签栈下发给转发器。  
                如上图所示，控制器下发标签栈的过程如下：  
                    1. 控制器将AS域内标签栈<102,203>和<405,506>分别下发给域内下发给隧道Ingress节点CSG1和ASBR3。  
                    2. 控制器将E2E SR-MPLS TE隧道标签栈<6000,3040,8000下发给跨域E2E隧道Ingress节点CSG1。  
                4. 转发器CSG1根据控制器下发的隧道配置和标签栈信息建立跨域E2E SR-MPLS TE隧道LSP。  

        * E2E SR-MPLS TE隧道数据转发  
            转发器上根据SR-MPLS TE隧道LSP对应的标签栈，对报文进行标签操作，并根据栈顶标签逐跳查找转发出接口，指导数据报文转发到隧道目的地址。  
            如图所示，控制器计算出SR-MPLS TE隧道路径为CSG1->AGG1->ASBR1->ASBR3->P1-PE1，对应标签栈<6000,3040,8000，其中6000和8000为Binding SID标签，3040为BGP Peer SID。  
            E2E SR-MPLS TE数据报文转发过程概述如下：  

            1. 入节点CSG1为数据报文添加标签栈<6000,3040,8000>，然后根据栈顶的标签6000查表，6000是Binding SID标签，对应标签栈<102,203>。然后根据102标签查找转发出接口，对应为CSG1->AGG1邻接，之后将标签102弹出。报文携带标签栈<203,3040,8000>，通过CSG1->AGG1邻接向下游节点AGG1转发。  
            2. AGG1收到报文后，根据栈顶的标签203匹配邻接，找到对应的转发出接口为AGG1->ASBR1邻接，之后将标签203弹出。报文携带标签栈<3040,8000>，通过AGG1->ASBR1邻接下下游节点ASBR1转发。  
            3. ASBR1收到报文后，根据栈顶的标签3040匹配邻接，找到对应的转发出接口为ASBR1->ASBR3邻接，之后将标签3040弹出。报文携带标签栈<8000>，通过ASBR1->ASBR3邻接向下游节点ASBR3转发。  
            4. ASBR3收到报文后，根据栈顶的标签8000查表，8000是Binding SID标签，对应标签栈<405,506>。然后根据405标签查找转发出接口，对应为ASBR3->P1邻接，之后将标签405弹出。报文携带标签栈<506>，通过ASBR3->P1邻接向下游节点P1转发。  
            5. P1收到报文后，根据栈顶的标签506匹配邻接，找到对应的转发出接口P1->PE1邻接，之后将标签506弹出。报文此时不携带标签，通过P1->PE1邻接向目的节点PE1转发。  

        * E2E SR-MPLS TE隧道可靠性  
            ***E2E SR-MPLS TE Hot-Standby***  
            E2E SR-MPLS TE隧道支持热备份(Hot-Standby),热备份是指创建主LSP后随即创建备份LSP，备份LSP始终处于热备状态。热备份LSP提供整条LSP的保护，是一种端到端的流量保护措施。  
            如图，首节点CSG1上配置Hot-Standby，这样CSG1在创建完Primary LSP后，立即创建Hot-Standby LSP也即一条SR-MPLS TE里包含两条LSP。当首节点感知到Primary LSP故障，会将流量切换到Hot-Standby LSP;如果Primary LSP故障恢复，流量切回到Primary LSP，整个过程中，SR-MPLS TE隧道始终是up状态。  
            ***E2E SR-MPLS TE隧道保护***  
            如图所示，E2E SR-MPLS TE隧道保护可以分为三个层次：  

            1. AS域内：如果E2E SR-MPLS TE LSP在AS域内出现故障，则优先通过域内SR-MPLS TE隧道的保护技术进行保护，例如域内SR-MPLS TE Hot-Standby与SR-MPLS TE FRR等。  
            2. E2E LSP级别：主要是指E2E SR-MPLS TE隧道内，Hot-Standby LSP为Primary LSP提供保护，报障E2E SR-MPLS TE隧道处于Up状态。Primary LSP可以通过单臂BFD for E2E SR-MPLS TE LSP进行检测，快速感知故障。  
            3. E2E隧道级别：如果E2E SR-MPLS TE隧道的Primary LSP和Hot-Standby LSP同时发生故障，则单臂BFD for E2E SR-MPLS TE隧道能够快速感知，系统将E2E SR-MPLS TE隧道置Down，此时VPN等上层应用可以通过VPN FRR切换到其他E2E SR-MPLS TE隧道。  

        * 单臂BFD for E2E SR-MPLS TE  
            E2E SR-MPLS TE由于没有协议建立，只要标签栈下发，LSP就会建立成功，且除了撤销标签栈之外，LSP不会出现协议Down的情况。所以E2E SR-MPLS TE LSP故障检测需要依靠部署BFD故障检测切换备份LSP。BFD for E2E SR-MPLS TE是一种端到端的快速检测机制，用于快速检测隧道所经过的链路汇总所发生的故障。BFD for E2E SR-MPLS TE提供以下几种方式:  

            + 单臂BFD for E2E SR-MPLS TE LSP：E2E SR-MPLS TE LSP建立时，如果BFD没有协商成功，则E2E LSP不UP。通过配置BFD for E2E SR-MPLS TE LSP，如果主路径故障可以快速切换到热备份(Hot-Standby)路径。  
            + 单臂BFD for E2E SR-MPLS TE隧道：E2E SR-MPLS TE隧道状态要结合BFD for E2E SR-MPLS TE Tunnel与BFD for E2E SR-MPLS TE LSP检测。  

                * BFD for E2E SR-MPLS TE LSP用来控制主备LSP的切换状态，BFD for E2E SR-MPLS TE Tunnel用来保证E2E Tunnel的真实状态。如果不配置BFD for E2E SR-MPLS TE Tunnel，Tunnel默认状态只为Up，Tunnel的真实状态不确定。  
                * E2E SR-MPLS TE隧道的接口状态依赖于BFD for E2E SR-MPLS TE Tunnel的检测，即E2E SR-MPLS TE Tunnel的接口状态与BFD状态一致，而BFD Up的时间由于BFD协议协商的要求会比较慢。一般情况下，当Tunnel状态为Down时下发新标签栈，到BFD Up需要十几秒的时间，这样会导致Tunnel在没有其他保护的情况下，硬收敛的时间较慢。  
            BFD for E2E SR-MPLS TE原理：  

                1. 首先需要在入节点配置使能BFD检测的同时指定单臂检测模式，建立BFD会话。  
                2. 其次出节点需要预先建立回程E2E SR-MPLS TE隧道，并为回程E2E SR-MPLS TE隧道配置Binding SID。  
                3. 最后在入节点配置BFD会话与回程E2E SR-MPLS TE隧道的Binding SID绑定。  

                单臂BFD会话建立后，在入节点发出单臂BFD报文，单臂BFD报文携带回程隧道的Binding SID。单臂BFD报文通过SR-MPLS TE隧道经中间节点到达出节点后，转发平面接收到BFD报文后，将MPLS标签剥离后，通过单臂BFD报文携带的Binding SID关联到回程SR-MPLS TE隧道，然后为回程BFD报文封装E2E SR-MPLS TE隧道标签栈，回程BFD报文通过SR-MPLS隧道经中间节点环回到入节点，入节点进行检测报文处理，实现单臂环回的检测机制。  

                对于出节点没有可用回程E2E SR-MPLS TE隧道的情况，出节点会根据BFD报文的目的地址查找IP路由进行环回转发。  

        * 跨多AS域E2E SR-MPLS TE  
            从前面章节描述可以看出，理论上利用Binding SID和BGP Peer SID直接建立显式路径就可以轻易跨越多个AS域(大于等于3个)。但是由于AS域通常隶属于不同的管理机构，跨多AS域时，很有可能就跨越了多个管理机构的网络，这在现实部署中可能存在一定障碍。  

            >PE1----ASBR1====ASBR3----ASBR5====ASBR7----PE3
            >PE2----ASBR2====ASBR4----ASBR6====ASBR8----PE4

            这是一个三个AS域相连的网络，如果单纯在PE1上直接使用多跳显式路径建立到PE3的E2E SR-MPLS TE网络，则在AS x中，就可以决定AS y到AS z的流量路径，由于AS y与AS z可能和AS x隶属于不同的管理机构，所以这样的流量路径可能并不为AS y和AS z接受，而且标签栈深度太深也会降低转发效率。另一方面，AS y与AS z也可能和AS x使用不同的控制器，这也给PE1直接建立到PE3的E2E SR-MPLS TE网络带来困难。  
            为了解决上述困难，设备做了限制，当显式路径第一跳是Binding SID时，该显式路径最多仅支持3跳，这样从PE1最多只能建立到ASBR5/ASBR6的跨域E2E SR-MPLS TE显式路径，而无法到达AS z。要想建立从AS x到AS z的跨多AS域E2E SR-MPLS TE隧道只能通过分层方式。  
            分层建立跨多AS域E2E SR-MPLS TE隧道过程如下：  

            1. 第一层：建立AS y到AS z的E2E SR-MPLS TE隧道。首先需要完成AS y和AS z域内SR-MPLS TE隧道创建，并且为域内隧道配置Binding SID，分别为Binding SID3和Binding SID5。其次在AS y和AS z域间配置BGP EPE，生成BGP Peer SID4之后控制器利用上述SID建立从AS y到AS z的跨域E2E SR-MPLS TE Tunnel1,可以为该隧道配置新的Binding SID6。  
            2. 第二层：建立AS x到AS z的E2E SR-MPLS TE隧道。首先需要完成AS x域内SR-MPLS TE隧道创建，并且为域内隧道配置Binding SID，名为Binding SID1。其次在AS x和AS y域间配置BGP EPE，生成BGP Peer SID2.之后控制器利用Binding SID1、Peer SID2和Binding SID6建立从AS x到AS z的单向跨域E2E SR-MPLS TE Tunnel2。  
            
            以上就是跨3个AS域的E2E SR-MPLS TE隧道建立过程。如果超过3个AS域，还可以为E2E SR-MPLS TE Tunnel2分配新的Binding SID，然后利用次SID参与路径计算。按上述思路循环，可以进一步构建跨更多AS域的E2E SR-MPLS TE隧道。  

    + 流量导入  
        在SR LSP(SR-MPLS BE)或者SR-MPLS TE隧道建立成功以后，还需要将业务流量引入SR LSP或者SR-MPLS TE隧道。常用放法有静态路由、隧道策略、自动路由等。常见业务有公网业务、EVPN、L2VPN和L3VPN等。  
        ***静态路由***  
        SR LSP无Tunnel接口。可以配置静态路由指定下一跳，根据下一跳迭代SR LSP。  
        SR-MPLS TE隧道上的静态路由没有什么特殊之处，它的工作方式和普通的静态路由一样。配置静态路由时，路由的出接口设置为SR-MPLS TE隧道的接口，即可按照该路由转发的流量导入SR-MPLS TE隧道。  
        ***隧道策略***  
        VPN流量通过隧道进行转发时，默认采用LDP LSP而非SR LSP或者SR-MPLS TE隧道。如果默认情况下不能满足VPN的需求，需要对VPN应用隧道策略，将VPN流量引入到SR LSP或者SR-MPLS TE隧道中。  
        目前隧道策略包含如下两种，可以根据需要选择其中一种进行配置。  

            * 按优先级顺序选择(Select-seq)方式：该策略可以改变VPN选择的隧道类型，按照配置的隧道类型优先级顺序将SR LSP或者SR-MPLS TE隧道选择为VPN的公网隧道。如果不存在LDP LSP，则默认选择SR LSP。  
            * 隧道绑定(Tunnel Binding)方式：该策略可以为VPN绑定SR-MPLS TE隧道以保证QoS，将某个目的地址与某条SR-MPLS TE隧道进行绑定。  
        ***自动路由***  
        自动路由是指将SR-MPLS TE隧道看作逻辑链路参与IGP路由计算，使用隧道接口做为路由出接口。根据网络规划来决定某节点设备是否将LSP链路发布给邻居节点用于指导报文转发，配置自动路由方式有两种：  

            * 转发捷径：此方式不将SR-MPLS TE隧道发布给邻居节点，因此，SR-MPLS TE隧道只能参与本地的路由计算，其他节点不能使用次隧道。  
            * 转发邻接：此方式将SR-MPLS TE隧道发布给邻居节点，因此，SR-MPLS TE隧道将参与全局的路由计算，其他节点也能使用此隧道。  

        ***策略路由***  
        策略路由根据用户制定的策略进行路由选择，可应用于安全、负载分担等目的。在SR网络中，可使符合过滤条件的IP报文通过指定的LSP路径转发。  
        SR-MPLS TE的策略路由的定义和IP单播策略路由完全一样，可以通过定义一系列匹配的规则和动作来实现，即将apply语句的出接口设置为SR-MPLS TE隧道的接口。如果报文不匹配策略路由规则，将进行正常IP转发;如果报文匹配策略路由规则，则报文直接从指定隧道转发。  

        * 公网IP迭代SR隧道  
            - BGP公网路由迭代SR隧道  
                当用户访问互联网时，如果报文采用IP转发的话，则从用户到互联网上的核心设备都需要学习到大量的互联网路由，这给核心设备带来了很大的负担，影响核心设备的性能。为了解决这个问题，可以在用户的接入设备上配置非标签公网BGP路由或者非标签公网静态路由迭代SR隧道功能，让用户通过SR隧道转发访问互联网，这样就能解决核心设备性能不足，压力过大或者不希望核心路由承载业务等问题。  
                按照下面的方式部署BGP公网路由迭代SR LSP:  

                1. PE、P端到端部属IGP和Segment Routing，建立SR LSP。  
                2. 在PE之间建立BGP Peer，学习对端路由。  
                3. BGP业务路由在PE1处迭代SR LSP。  

                >Internet==PE1==P1==P2==PE2==CE  
                >==========||------BGP--------||====  
                >ip->26100+ip->36100+ip->16100+ip->ip  

                按照下面的方式部署BGP公网路由迭代SR-MPLS TE隧道:  

                1. PE、P端到端部署IGP和Segment Routing，建立SR-MPLS TE隧道。  
                2. 在PE之间建立BGP Peer，学习对端路由。  
                3. 在PE上配置隧道策略，使BGP业务路由在PE1处迭代SR-MPLS TE隧道。  

                >Internet==PE1=9001=P1=9002=P2=9003=PE2==CE  
                >==========||-------------BGP-------------||==========  
                >ip+(9003,9002,9001)->ip+(9003,9002)->ip+(9003)->ip->ip  

            - 静态路由迭代SR隧道  
                静态路由的下一跳有可能不是直接可达的，这样的路由不能指导转发，需要进行迭代。如果允许静态路由迭代SR隧道，则可以进行标签转发。  
                按照下面的方式部署静态路由迭代SR LSP：  

                1. PE、P端到端部署IGP和Segment Routing，PE1建立到PE2目的地址为Loopback接口地址的SR LSP。  
                2. 在PE1上配置静态路由，指定下一跳为PE2的Loopback接口地址。  
                3. PE1收到IP报文后封装标签，通过SR LSP进行报文转发。  

                >Internet==PE1==P1==P2==PE2==CE  
                >==========||-pe2 static route-||===
                >ip->ip+26100->ip+36100->ip+16100->ip  

                按照下面的方式部署静态路由迭代SR-MPLS TE隧道  

                1. PE、P端到端部署IGP和Segment Routing，PE1建立到PE2目的地址为Loopback接口地址的SR-MPLS TE隧道。  
                2. 在PE1上配置静态路由，指定下一跳为PE2的Loopback接口地址。  
                3. 在PE上配置隧道策略，使PE迭代到SR-MPLS TE隧道，之后PE1收到IP报文后将进行标签转发。  

                >Internet==PE1=9001=P1=9002=P2=9003=PE2==CE  
                >==========||-Loopback static route-||=====  
                >ip->ip+(9003,9002,9001)->ip+(9003,9002)->ip+(9003)->ip->ip  

        * L3VPN迭代SR隧道  
            - 基本VPN迭代SR隧道  
                当用户访问互联网时，如果报文采用IP转发的话，则从用户到互联网的核心设备都需要学习到大量的互联网路由，这给核心设备带来了很大的负担，影响核心设备的性能。为了解决这个问题，可以配置VPN迭代SR隧道功能，让用户通过隧道访问互联网。  
                网络是由一些不连续的L3子网VPN组成，中间跨域骨干网络，在PE之间建立SR LSP，用于转发L3VPN私网报文。PE设备使用BGP学习私网VPN路由。具体部属如下：  

                1. PE两端部署IS-IS，实现路由可达。  
                2. PE两端建立BGP邻居，学习对端VPN路由。  
                3. PE建立ISIS SR隧道，实现公网标签分配，并计算标签转发路径。  
                4. 通过BGP协议为VPN分配一个私网标签，例如Label Z。  
                5. 私网路由迭代到SR LSP。  
                6. PE1收到IP报文，封装私网标签，封装SR公网标签，按照标签转发路径进行转发。  

                >CE1==PE1==P1==P2==PE2==CE2  
                >=====||----BGP----||======    
                >ip->ip+(Label Z+26100)->ip+(Label Z+36100)->ip+(Label Z)PHP->ip  

                网络是由一些不连续的L3子网VPN组成，中间跨域骨干网络，在PE之间建立SR-MPLS TE隧道，用于转发L3VPN私网报文。PE设备使用BGP学习私网VPN路由。具体部署如下：  

                1. PE两端部署IS-IS，实现路由可达。  
                2. PE两端建立BGP邻居，学习对端VPN路由。  
                3. PE建立IS-IS SR-MPLS TE隧道，实现公网标签分配，并计算标签转发路径。  
                4. 通过BGP协议为VPN分配一个私网标签，列入Label Z。  
                5. PE上配置隧道策略，允许私网路由迭代到SR-MPLS TE隧道。  
                6. PE1收到IP报文，封装私网标签，封装SR公网标签，按照标签转发路径进行转发。  

                >CE1==PE1=9001=P1=9002=P2=9003=PE2==CE2  
                >=====||----------BGP----------||======  
                >ip->ip+(Label Z+9003,9002,9001)->ip+(Label Z+9003,9002)->ip+(Label Z+9003)->ip+(Label Z)->ip  

            - HVPN迭代SR隧道  
                随着网络规模的不断扩大，业务种类不断增加，部署PE设备经常会遇到接入能力或者路由能力的扩展性问题，从而导致整个网络的性能和可扩展性将受到影响，不利于大规模部署VPN。HVPN是具有层次化的VPN网络，由多个PE承担不同的角色，并形成层次结构，共同完成一个PE的功能，以降低对PE设备的性能要求。  

                1. UPE/SPE/NPE设备之间建立BGP Peer，并建立SR LSP  
                2. 在UPE设备，将VPNv4路由迭代到SR LSP.  

                HVPN报文转发过程(CE1向CE2发送报文)如下：  

                1. CE1向UPE发送一个VPN报文。  
                2. UPE收到报文后，查询私网转发表，通过匹配报文目的地址，查找到用于转发报文的隧道，然后给报文打上私网标签L4和外层SR公网标签Lv，通过隧道将报文发给SPE，标签栈为L4/Lv;  
                3. SPE收到报文后，弹出外层SR公网标签Lv，交换私网标签为L3,打上外层SR公网标签Lu，沿隧道将报文转发给NPE，标签栈为L3/Lu;  
                4. NPE收到报文后，弹出外层SR公网标签Lu，根据内层标签L3找到对应的VPN实例，然后根据报文目的地址在该VPN实例的私网转发表中查找到出接口。NPE将报文从对应出接口发送给CE2.此时报文为纯IP报文。  

                >VPN1 Site1==CE1==UPE==SPE==NPE==CE2==VPN1 Site2  
                >================||-BGP-||-BGP-||===============  
                >Payload->Payload+(L4+Lv)->Payload+(L3+Lu)->Payload  

            - VPN FRR  
                PE1设备会同时收到PE3和PE4发布的路由，并且优选PE3发布的路由，PE4发布的路由做为次优路由。PE1将最优路由和次优路由都要填写在转发项中，其中最优路由用于指导业务流量转发，而次优路由作为备份路由。  

                >====PE1==P1==P3==PE3====  
                >CE1=||===||==||===||=CE2  
                >====PE2==P2==P4==PE4====  

                |故障点|保护倒换|  
                |:--:|:--:|  
                |P1->P3链路故障|TI-LFA局部保护生效，流量切换到PE1-P1-P2-P4-P3-PE3|
                |PE3接点故障|BFD for SR-MPLS TE或SBFD for SR-MPLS可以感知，BFD/SBFD会触发VPN FRR切换到PE1-PE2-P2-P4-PE4|  

        * L2VPN迭代SR隧道  
            VPLS的典型组网，处于不同物理位置的用户同国接入不同的PE设备，实现用户之间的互相通信。从用户的角度来看，整个VPLS网络就是一个二层交换网，用户之间就像直接通过LAN互连在一起一样。用户通过配置VPLS迭代SR隧道，每个VPN的各个Site之间建立虚连接，公网SR隧道建立以后，二层报文就可以进入SR隧道转发。  

            - VPLS迭代SR LSP  
                VPLS迭代SR LSP流程如下：  

                1. CE1发送经过二层封装的报文到PE1。  
                2. PE1建立到PE2的端到端SR LSP。  
                3. PE1配置隧道策略，选择SR LSP，VSI转发表项关联到SR转发表项。  
                4. PE1收到报文，查找对应的VSI中的表项，为该报文选择隧道和PW。PE1根据选择的隧道和PW，为该报文直接打上外层LSP标签和内层VC标签，再进行二层封装后转发。  
                5. PE2收到从PE1发送来的报文，对该报文进行解封装。  
                6. PE2把解封装后的CE1原始二层报文发送给CE2。  

                >VPN1 Site1==CE1==PE1==P1==P2==PE2==CE2==VPN1 Site2  
                >=================||--SR LSP+PW--||================  
                >ip+L2 head->ip+L2 head+(VC Label+16100)+L2 head->ip+L2 head+(VC Label+16100)+L2 head->ip+L2 head+(VC Label)+L2 head PHP->ip+L2 head  

                HVPLS迭代SR LSP、VLL迭代SR LSP场景与VPLS迭代SR LSP流程类似。  


            - VPLS迭代SR-MPLS TE隧道
                VPLS迭代SR-MPLS TE隧道流程如下：  

                1. CE1发送经过二层封装的报文到PE1。  
                2. PE1建立到PE2的端到端SR-MPLS TE隧道。  
                3. PE1配置隧道策略，选择SR-MPLS TE隧道，VSI转发表项关联到SR转发表项。  
                4. PE1收到报文，查找对应的VSI中的表项，为该报文选择隧道和PW。PE1根据选择的隧道和PW，为该报文直接打上外层SR-MPLS TE隧道标签和内层VC标签，再进行二层封装后转发。  
                5. PE2收到从PE1发送来的报文，对该报文进行解封装。  
                6. PE2把解封装后的CE1原始二层报文发送给CE2。  

                >VPN1 Site1==CE1==PE1=9001=P1=9002=P2=9003=PE2==CE2==VPN1 Site2  
                >=================||--SR-MPLS TE--||===============  
                >ip+L2 head->ip+L2 head+(VC Label+9003,9002,9001)+L2 head->ip+L2 head+(VC Label+9003,9002)+L2 head->ip+L2 head+(VC Label+9003)+L2 head->ip+L2 head+(VC Label)+L2 head->ip+L2 head  

        * EVPN迭代SR隧道
            EVPN(Ethernet Virtual Private Network)是一种用于二/三层网络互联的VPN技术。EVPN技术采用类似于BGP/MPLS IP VPN的机制，通过扩展BGP协议，使用扩展后的可达性信息，使不同站点的二层网络间的MAC地址学习和发布过程从数据平面转移到控制平面。与VPLS相比，EVPN技术可以解决VPLS技术存在的无法实现负载分担、网络资源消耗高的问题。  
            - EVPN迭代SR LSP  
                当PE学到其他站点的MAC地址且公网SR LSP建立成功后，则可以向其他站点传输单播报文，其具体转发过程如下：  

                1. CE1将单播报文以二层转发的方式发送至PE1； 
                2. PE1收到报文后，先封装MAC表项携带的私网标签，在封装公网SR LSP标签，然后将封装后的单播报文发送至PE2;  
                3. PE2收到封装后的单播报文后，将进行解封装，弹出私网标签，查对应私网MAC表，找到相应的出接口。  

                >EVPN1 Site1==CE1==PE1==P1==P2==PE2==CE2==EVPN1 Site2  
                >==================||----SR LSP--||==================  
                >L2 Payload->L2 Payload+(Private Label+16100)->L2 Payload+(Private Label+16100)->L2 Payload+(Private Label) PHP->L2 Payload  

            - EVPN迭代SR-MPS TE隧道  
                当PE学到其他站点的MAC地址且公网SR-MPLS TE隧道建立成功后，则可以向其他站点传输单播报文，其具体传输过程如下：  

                1. CE1将单播报文以二层转发的方式发送至PE1； 
                2. PE1收到报文后，先封装MAC表项携带的私网标签，再封装公网SR-MPLS TE隧道标签，然后将封装后的单播报文发送至PE2;  
                3. PE2收到封装后的单播报文后，将进行解封装，弹出私网标签，查对应私网MAC表，找到相应的出接口。  

                >EVPN1 Site1==CE1==PE1=9001=P1=9002=P2=9003=PE2==CE2==EVPN1 Site2  
                >==================||-------SR-MPLS TE-------||============  
                >L2 Payload->L2 Payload+(Private Label+9003,9002,9001)->L2 Payload+(Private Label+9003,9002)->L2 Payload+(Private Label+9003)->L2 Payload+(Private Label)->L2 Payload  

    + SBFD for SR-MPLS
        BFD技术相对成熟，但当配置大量BFD会话进行链路检测时，BFD现有状态机的协商时间会变长，成为整个系统的一个瓶颈。SBFD(Seamless Bidirectional Forwarding Detection)是BFD的一种简化机制，SBFD简化了BFD的状态机，缩短了协商时间，提高了整个网络的灵活性，能够支撑SR隧道检测。  

        + SBFD工作原理  
            SBFD工作原理如图所示。SBFD分为发起端和反射端，在链路检测之前，发起端和反射端通过互相发送SBFD控制报文(SBFD Control Packet)通告SBFD描述符(Discriminator)等信息。链路检测时，发起端主动发送SBFD Echo报文，反射端根据本端情况环回此报文，发起端根据反射报文决定本端状态。  

            * 发起端做为检测端，有SBFD状态机机制和检测机制。发起端状态机只有Up和Down状态，发出的报文也只有Up和Down状态，只能接收Up或Admin Down状态报文。  
            SBFD报文由发起端首先向反射端发送，报文初始状态为Down。报文目的端口号为7784,报文的源端口号为4784,目的IP为用户配置的127网段的任意IP地址，源IP地址为本端设备上配置的LSR ID。  
            * 反射端无SBFD状态机，无检测机制，不会主动发送SBFD Echo报文，仅用于构造环回SBFD报文。  
            反射端接收到发起端的SBFD报文，检查报文中SBFD描述符是否与本地配置的全局SBFD描述符匹配，不匹配则丢弃;如果匹配并且反射端处于工作状态，则构造环回SBFD报文，反射端不处于工作状态，则将报文状态置为Admin Down。  
            反射端所构造报文的目的端口号为4784,报文的源端口号为7784,源IP地址为本端设备配置的LSR ID，目的IP为发起端的源IP地址。  

            >发起端==================反射端  
            >||--SBFD Control packet-->||
            >||<--SBFD Control pcaket--||
            >||----SBFD Echo packet----||
            >||<-----------------------||

        + SBFD发起端状态机  
            SBFD发起端状态机只有Up和Down两个状态，也只能在这两个状态间转变，如下图：  

            >||--------Up--------||  
            >Down================UP  
            >||-Admin Down,Timer-||  

            1. 初始状态：SBFD报文由发起端首先向反射端发送，发起端初始状态为Down。  
            2. 状态迁移：发起端收到反射端发回的Up报文则将本地状态置为Up。发起端收到反射端返回Admin Down报文，则将状态置为Down。发起端在定时器超时前收不到返回报文，也将状态置为Down。  
            3. 状态保持：发起端处于Up状态，如果收到反射端返回的Up报文，则本地状态持续维持在Up状态。发起端处于Down状态，如果收到反射端返回的Admin Down报文或者在定时器超时前收不到返回报文，则本地状态持续维持在Down状态。  

        + SBFD典型应用  
            SBFD应用到SR场景检测时，主要有SBFD for SR LSP(SR-MPLS BE)和SBFD for SR-MPLS TE两种场景。在SBFD检测SR-MPLS场景，SBFD发起端到反射端路径走SR-MPLS标签路径转发，反射端向发起端回程报文走多跳IP路径。  

            1. SBFD for SR LSP  
                下面以VPN迭代到SR LSP为例，描述SBFD for SR LSP的应用场景，如下图所示：  
                图中所有PE和P设备上SRGB范围都是[16100-16100]。A、CE1、CE2、E部署在同一个VPN中，CE2发布一条到E的路由。PE2为到E的路由分配VPN标签，然后同国MP-BGP邻居发布给PE1,PE1接收携带VPN标签的VPN路由，并将路由下发到转发表。  
                当A发送目的地为E的报文时，报文在PE1节点根据报文所属VPN封装VPN标签，根据报文目的地址迭代SR LSP，然后封装SR标签16100,之后报文沿着主路径PE1->P4->P3->PE2进行逐跳转发。  
                应用了SBFD检测后，当主路径某条链路或者某台P设备发生故障时，PE1会快速检测到故障发生，使数据流量切换到备份SR LSP。  

                >A========P1=(Backup SR LSP)=P2========E  
                >CE1==PE1=======================PE2==CE2  
                >=========P4=(Primary SR LSP)=P3========  
                >Payload+IP head->Payload+IP head+(VPN label+16100)->...->Payload+IP head  

            2. SBFD for SR-MPLS TE LSP
                下面以VPN迭代到SR-MPLS TE LSP为例，描述SBFD for SR-MPLS TE LSP的应用场景，如下图所示：  

                >A========P1=(Backup SR LSP)=P2==========E  
                >CE1==PE1=========================PE2==CE2  
                >=========P4=(Primary SR LSP)=P3==========  
                >Payload+Ip head->Payload+Ip head+(VPN label+9005,9003,9004)->Payload+Ip head+(VPN label+9005,9003)->Payload+Ip head+(VPN label+9005)->Payload+Ip head+(VPN label)->Payload+IP head  

                A、CE1、CE2、E部署在同一个的VPN中，CE2发布一条到E的路由。PE2为E分配VPN标签。PE1安装到E的路由，VPN标签。  
                PE1到PE2的SR-MPLS TE Tunnel的路径为PE1->P4->P3->PE2,标签栈为{9004,9003,9005}。当A发送目的地为E的报文时，报文在PE1时根据9004找到出接口，发出时被打上三层标签，9003、9005和最内层的VPN标签为PE2分配的。应用了SBFD检测后，当主路径某条链路或者某台P设备发生故障时，PE1会快速检测到故障发生，并将数据流量切换到备份SR-MPLS TE LSP。  

            3. SBFD for SR-MPLS TE Tunnel  
                SR-MPLS TE隧道状态要结合SBFD for SR-MPLS TE Tunnel与SBFD for SR-MPLS TE LSP检测。  
                SBFD for SR-MPLS TE LSP用来控制主备LSP的切换状态，SBFD for SR-MPLS TE Tunnel用来保证Tunnel的真实状态。  

                * 如果不配置SBFD for SR-MPLS TE Tunnel，Tunnel默认状态只为UP，Tunnel的真实状态不确定。  
                * 如果配置了SBFD for SR-MPLS TE Tunnel，但是SBFD的状态被置为管理Down，则SBFD实际未工作，隧道接口状态是unknown。  
                * 如果配置了SBFD for SR-MPLS TE Tunnel，且SBFD没有被置为管理Down，则隧道接口状态与BFD状态一致。  

    + SR-MPLS TE Policy  
        SR-MPLS TE Policy是在SR技术基础上发展的一种新的隧道引流技术。SR-MPLS TE Policy路径表示为指定路径的段列表(Segment List)，称为SID列表(Segment ID List)。每个SID列表是从源到目的地的端到端路径，并指示网络中的设备遵循指定的路径，而不是遵循IGP计算的最短路径。如果数据包被导入SR-MPLS TE Policy中，SID列表由头端添加到数据包上，网络的其余设备执行SID列表中嵌入的指令。  
        SR-MPLS TE Policy包括以下三个部分：  

        * 头端(HeadEnd):SR-MPLS TE Policy生成的节点。  
        * 颜色(Color):SR-MPLS TE Policy携带的扩展团体属性，携带相同Color属性的BGP路由可以使用该SR-MPLS TE Policy。  
        * 尾端(EndPoint):SR-MPLS TE Policy的目的地址。  

        Color和End Point信息通过配置添加到SR-MPLS TE Policy。SR-MPLS TE Policy在算路时按照代表业务SLA要求的Color属性计算转发路径，业务网络头端通过路由携带的Color属性和下一跳信息来匹配对应的SR-MPLS TE Policy实现业务流量转发。Color属性定义了应用级的网络SLA策略，可基于特定业务SLA规划网络路径，实现业务价值细分，构建新的商业模式。  

        ***SR-MPLS TE Policy模型***
        SR-MPLS TE Policy模型如下图所示。一个SR-MPLS TE Policy可以包含多个候选路径(Candidate Path)。候选路径携带优先级属性(Preference)。优先级最高的有效候选路径做为SR-MPLS TE Policy的主路径，优先级次高的有效路径做为SR-MPLS TE Policy的热备份路径。  
        一个候选路径可以包含多个Segment List，每个Segment List携带Weight属性(当前不支持设置，默认为1)。每个Segment List都是一个显式标签栈，Segment List可以指示网络设备转发报文。多个Segment List之间可以形成负载分担。  

        >==============================================|-Segment List 11+Weight
        >==================|-Candidate Path+Preference |====================
        >==================|===========================|-Segment List 1m+Weight
        >SR-MPLS TE Policy |===================================================
        >==================|===========================|-Segment List n1+Weight
        >==================|-Candidate Path+Preference |=======================
        >==================|===========================|-Segment List nm+weight

        ***BGP SR-MPLS TE Policy扩展***  
        BGP SR-MPLS TE Policy扩展有两个主要用途  
        
        1. 控制器上动态生成的SR-MPLS TE Policy需要通过BGP扩展传递给转发器。该功能通过新定义的SAFI，即BGP SR-MPLS TE Policy地址族来实现。在控制器和转发器之间建立BGP SR-MPLS TE Policy地址族邻居，可以让控制器下发SR-MPLS TE Policy给转发器，提高了网络自动化部署SR-MPLS TE Policy的能力。  
        2. 支持新增的扩展团体属性(Color属性)，并且可以在BGP邻居之间传递。该功能通过BGP NLRI扩展来实现。  

        BGP SR-MPLS TE Policy地址族使用的NLRI格式如下：  

        >NLRI Length(1 octet)+Distinguisher(4 octets)+Policy Color(4 octets)+Endpoint(4 or 16 octets)  

        包含SR-MPLS TE Policy的NLRI在BGP Update消息中携带，Update消息也必须携带BGP必选属性，另外，也可以携带任何BGP可选属性。  
        SR-MPLS TE Policy的内容在隧道封装属性(Tunnel Encapsulation Attribute)中使用新的隧道类型TLV(Tunnel-Type TLV)进行编码。  

        Tunnel-Type TLV包含有多个Sub-TLV。  

        * Preference Sub-TLV  
        * Binding SID Sub-TLV  
        * Segment List Sub-TLV  
        * Policy Priority Sub-TLV  
        * Extended Color Community  

        BGP本身不产生SR-MPLS TE Policy，可以从控制器接收SR-MPLS TE Policy NLRI，并且生成SR-MPLS TE Policy。BGP接收SR-MPLS TE Policy NLRI时，需要做以下检查：  

        * SR-MPLS TE Policy NLRI必须包含Distinguisher、Policy Color和Endpoint。  
        * SR-MPLS TE Policy NLRI的Update消息必须包含NO_ADVERTISE团体属性，或者包含IPv4地址形式的route-target扩展团体属性。  
        * SR-MPLS TE Policy NLRI的Update消息必须包含隧道封装属性(Tunnel Encapsulation Attribute),而且必须有一个隧道封装属性(Tunnel Encapsulation Attribute)。  

        ***SR-MPLS TE Policy的MTU***  
        如果一个SR-MPLS TE Policy的实际转发路径中有不同的物理链路类型，可能不同的链路支持的MTU各不相同，就需要在头节点做好MTU控制，防止报文在传输过程中被切片或丢弃。目前IGP/BGP并没有实现对MTU的传递或协商的机制，用户可以为SR-MPLS TE Policy单独配置MTU。  
        ***SR-MPLS TE Policy的MBB与慢删机制***  
        SR-MPLS TE Policy支持MBB(make before break)。在SR-MPLS TE Policy的Segment List更新过程中，转发器在拆除老的Segment List之前先把新的Segment List建立起来，这期间流量先保持按照老的Segment List转发，防止路径切换导致流量丢失。  
        SR-MPLS TE Policy支持慢删机制。配置Segment List延迟删除时间后，如果SR-MPLS TE Policy下有更优的Segment List，系统可能会进行Path切换，此时SR-MPLS TE Policy下原有处于Up状态的Segment List仍旧可以使用，系统将等待延迟时间超时以后才将原有Segment List删除，防止Path切换震荡导致流量不通。  
        慢删机制仅对SR-MPLS TE Policy下处于Up状态的Segment List(包括备份Segment List)生效。如果SBFD检测到Segment List故障，或者没有获取到Segment List真实状态，则系统认为Segment List不可用，此时会立即删除Segment List，而不进行任何延时。  

        1. SR-MPLS TE Policy创建  
            SR-MPLS TE Policy可以在转发器上静态配置(CLI/Netconf)，也可以由控制器上动态生成然后传递给转发器(BGP)。相对来说，动态方式更利于网络自动化部署。存在多个方式产生的相同SR-MPLS TE Policy时，转发器按照如下原则选择SR-MPLS TE Policy。  

            * SR-MPLS TE Policy的来源(protocol-Origin):通过BGP接收的SR-MPLS TE Policy默认取值是20,静态配置的SR-MPLS TE Policy默认取值是30。取值越大，优先级越高。  
            * SR-MPLS TE Policy的<ASN,node-address>:其中ASN是AS编号。ASN和node-address都是取值越小，优先级越高。对于静态配置的SR-MPLS TE Policy，目前ASN固定取值是0;node-address固定取值是0.0.0.0。  
            * Discriminator：取值越大，优先级越高。对于静态配置的SR-MPLS TE Policy，Discriminator固定取preference的值。  

            ***静态配置SR-MPLS TE Policy***  
            用户可以通过CLI/Netconf静态配置SR-MPLS TE Policy。其中静态配置SR-MPLS TE Policy时，Endpoint、Color、候选路径的Preference和Segment List等都是必须配置。静态配置时，同一个SR-MPLS TE Policy下候选路径的Preference不允许重复。  
            Segment List第一跳标签支持Node标签、Adjacency标签，以及BGP EPE标签、并行标签、Anycast SID标签，不支持Binding SID。需要特别注意的是，Segment List的第一跳标签必须是本地的入标签，转发平面会根据这个标签值在本地查找转发表项。  

            ***控制器下发SR-MPLS TE Policy***  
            控制器下发SR-MPLS TE Policy过程如下：

            1. 控制器通过BGP-LS收集网络拓扑和标签信息。  
            2. 控制器与头端转发器之间建立IPv4 SR-MPLS TE Policy地址族的BGP邻居。  
            3. 控制器计算SR-MPLS TE Policy，然后通过BGP邻居下发给头端。头端生成SR-MPLS TE Policy表项。  

        2. SR-MPLS TE Policy流量导入  
            ***路由着色***  
            路由着色是指通过路由策略对路由增加扩展团体属性Color，携带Color的路由可以根据Color与下一跳地址迭代SR-MPLS TE Policy。  
            路由着色的过程如下：  

            1. 配置路由策略，匹配特定路由，设置特定的Color属性。  
            2. 将路由策略应用到BGP邻居，或者应用到整个VPN实例，可以作为入口策略，也可以作为出口策略。  

            ***Color引流***  
            Color引流时，直接基于路由的扩展团体属性Color和目的地址迭代到SR-MPLS TE Policy。  
            整个过程简述如下：  

            1. 通过控制器向头端设备A下发SR-MPLS TE Policy，SR-MPLS TE Policy的Color是123,EndPoint是设备B的地址10.1.1.3。  
            2. 在设备B上配置BGP出口策略或者VPN出口策略(也可以在设备A上配置BGP入口策略或者VPN入口策略)，为路由前缀10.1.1.0/24设置扩展团体属性Color 123，路由下一跳是设备B的地址10.1.1.3。然后路由通过BGP邻居发送给设备A。  
            3. 在头端设备A上配置隧道策略，然后当设备A接收到BGP路由10.1.1.0/24后，根据路由的扩展团体属性123和下一跳10.1.1.3迭代到SR-MPLS TE Policy。转发时，为到10.1.1.0/24的报文添加一个具体的标签栈<C，E，G，B>。  

            ***DSCP引流***  
            DSCP引流时，不支持按照Color进行路由迭代，而是根据路由的目的地址迭代隧道策略，利用Endpoint匹配到SR-MPLS TE Policy Group，然后再根据报文携带的DSCP查找到对应的SR-MPLS TE Policy转发。  

        3. SR-MPLS TE Policy数据转发  
            ***域内数据转发***  
            域内数据转发依赖于域内SR-MPLS TE Policy转发表项的建立，域内SR-MPLS TE Policy转发表项的建立过程如下：  

            1. 控制器下发SR-MPLS TE Policy给头端A设备。  
            2. 尾端B设备发布BGP路由10.1.1.0/24给头端A设备，BGP路由的下一跳是B设备的地址10.1.1.3/32。  
            3. 在头端A设备上配置隧道策略。A设备在接收到BGP路由以后，利用路由的Color和下一跳迭代到SR-MPLS TE Policy，SR-MPLS TE Policy的标签栈是<20003,20005,20007,20002>。  

            数据转发的过程如下：头端A设备查找到SR-MPLS TE Policy，然后为到10.1.1.0/24的报文压入标签栈<20003,20005,20007,20002>。设备C，E，G收到SR-MPLS TE报文后，都进POP操作，最终由G设备将报文转发给B设备。B设备收到报文后，根据报文的目的地址做进一步处理。  

            ***跨域数据转发***  
            跨域数据依赖于跨域SR-MPLS TE Policy转发表项的建立，跨域SR-MPLS TE Policy建立过程如下：  

            1. 控制器向AS 200头端设备ASBR3下发域内的SR-MPLS TE Policy，SR-MPLS TE Policy的Color是123，EndPoint是PE1的地址10.1.1.3，Binding SID是30028。  
            2. 控制器向AS 100头端设备CSG1下发跨域的E2E SR-MPLS TE Policy，SR-MPLS TE Policy的Color是123,EndPoint是PE1的地址10.1.1.3。其中Segment List将AS 100域内标签，域间BGP Peer-Adj标签和AS 200的Binding SID标签组合起来，即<102,203,3040,30028>  
            3. 在PE1上配置BGP出口策略或者VPN出口策略，为路由前缀10.1.1.0/24设置Color数性23,路由下一跳是PE1的地址10.1.1.3。然后路由通过BGP邻居发布给CSG1。  
            4. 在头端设备CSG1上配置隧道策略。CSG1接收到BGP路由10.1.1.0/24后，利用路由的Color和下一跳迭代到E2E SR-MPLS TE Policy，SR-MPLS TE Policy的标签栈是<102,203,3040,30028>。  

            跨域数据转发过程如图所示：  

            1. 入节点CSG1为数据报文添加标签栈{102,203,3040,30028}。然后根据102标签查找转发出接口，对应为CSG1->AGG1邻接，之后将标签102弹出。报文携带标签栈<203,3040,30028>，通过CSG1->AGG1邻接向下游节点AGG1转发。  
            2. AGG1收到报文后，根据栈顶的标签203匹配邻接，找到对应的转发出接口为AGG1->ASBR1邻接，之后将标签203弹出。报文携带标签栈<3040,30028>，通过AGG1->ASBR1邻接向下游节点ASBR1转发。  
            3. ASBR1收到报文后，根据栈顶的标签3040匹配邻接，找到对应的转发出接口为ASBR1->ASBR3邻接，之后将标签3040弹出。报文携带标签栈<30028>，通过ASBR1->ASBR3邻接向下游节点ASBR3转发。  
            4. ASBR3收到报文后，根据栈顶的标签30028查表，30028是Binding SID标签，对应标签栈<405,406>。然后根据405标签查找转发出接口，对应为ASBR3->P1邻接，之后将标签405弹出。报文携带标签栈<506>，通过ASBR3->P1邻接向下游节点P1转发。  
            5. P1收到报文后，根据栈顶的标签506匹配邻接，找到对应的转发出接口为P1->PE1邻接，之后将标签506弹出。报文此时不携带标签，通过P1->PE1邻接向目的节点PE1转发。PE1收到报文后根据报文的目的地址做进一步处理。  

        4. SBFD for SR-MPLS TE Policy  
            与RSVP-TE可以通过转发器相互发送Hello消息维持隧道状态不同，SR-MPLS TE Policy不会通过转发器之间互相发送消息来维持自身状态。只要头节点标签栈下发，SR-MPLS TE Policy就会建立成功，且除了撤销标签栈之外，SR-MPLS TE Policy不会出现Down的情况。所以SR-MPLS TE Policy故障检测需要依靠部署SBFD检测，通过SBFD故障检测切换备份路径。SBFD for SR-MPLS TE Policy是一种端到端的快速检测机制，用于快速检测SR-MPLS TE Policy所经过的链路中所发生的故障。  
            SBFD for SR-MPLS TE Policy检测过程如下：  

            1. 头节点使能SBFD for SR-MPLS TE Policy后，头节点默认将EndPoint地址(只支持IPv4地址)作为对应Segment List的SBFD的远端描述符。SR-MPLS TE Policy不存在多个Segment List时，则多个SBFD会话的远端描述符相同。  
            2. 头节点对外发送SBFD报文，SBFD报文封装SR-MPLS TE Policy对应的标签栈。  
            3. 尾节点收到SBFD报文后，通过IP链路按照最短路径发送回应报文。  
            4. 头节点如果收到SBFD回应报文，则认为SR-MPLS TE Policy的Segment List正常，否则会认为Segment List故障。如果一个候选路径下所有Segment List都发生故障，则SBFD触发候选路径切换。  

            由于SBFD返程走IP转发，所以当两点间有多个SR-MPLS TE Policy主路径因为约束条件各不相同的时侯，SBFD检测报文返程走的路径却是共享的，返程的路径故障容易引起所有经过的SBFD报Down，进而导致两点间的所有SR-MPLS TE Policy Down。同一个SR-MPLS TE Policy下多个Segment List的SBFD会话存在同样的问题。  
            默认情况下，如果SR-MPLS TE Policy没有使能HSB保护，SBFD仅检测SR-MPLS TE Policy里优先级最高的Candidate Path里的所有Segment List。使能HSB保护以后，SBFD可以同时检测SR-MPLS TE Policy里优先级最高和次高的两条Candidate Path里的所有Segment List，如果优先级最高的Candidate Path里所有Segment List都产生故障，将触发HSB切换。  

        5. SR-MPLS TE Policy故障切换  
            通过SBFD for SR-MPLS TE Policy 可以检测Segment LIst的可靠性，如果Segment List发生故障，将触发SR-MPLS TE Policy的鼓掌切换。  
            如图所示，SR-MPLS TE Policy1的头端是PE1，尾端是PE2。另外SR-MPLS TE Policy2的头端是PE1，尾端是PE3。SR-MPLS TE Policy1和SR-MPLS TE Policy2可以形成VPN FRR。SR-MPLS TE Policy1下形成了主path和备份path组成的Hot-standby保护。Segment List指定的是到P1、P2、PE2的Node标签，Segment List 1可以直接使用SR-MPLS的所有保护技术，例如TI-LFA FRR。  

            >================P5(20011)==P6(20012)====================
            >=========1==P1(20003)2=3=P2(20005)4=5=PE2(20007)========
            >CE1==PE1(20001)===============================CE2=======
            >============P3(20004)====P4(20006)====PE3(20008)========

            SR-MPLS TE Policy标签栈信息  

            >VPN FRR---SR-MPLS TE Policy1|--Main path---Segment List 1---TI-LFA
            >============================|--Backup path---Segment List 2=======
            >=======|--SR-MPLS TE Policy2---Main Path---Segment List 3=========

            * 当故障点1、3、5故障时：PE1、P1、P2上TI-LFA局部保护生效。PE1上对应Segment List1的SBFD如果在局部保护恢复流量之前就检测到故障，则SBFD联动将Setment List1置Down，并且通知SR-MPLS TE Policy1切换到备份Path Segment List2。  
            * 当故障点2、4故障时：无局部保护，依赖SBFD感知节点故障，并将Segment List1置Down，SR-MPLS TE Policy1切换到备份Path Segment List2。  
            * 当PE2节点故障时：SR-MPLS TE Policy1的所有候选Path都不可用，则SBFD可以感知，并且将SR-MPLS TE Policy1置Down，同时触发VPN FRR切换到SR-MPLS TE Policy2。  

        6. SR-MPLS TE Policy OAM
            SR-MPLS TE Policy OAM(Operations,Administration,and Maintenance)主要用于监控SR-MPLS TE Policy的连通性，快速进行SR-MPLS TE Policy的故障检测。SR-MPLS TE Policy OAM当前主要通过Ping/Tracert来实现。  
            SR-MPLS TE Policy是对与Path的策略控制，Path是真正的SID栈路径。SR-MPLS TE Policy的检测有两个层次：  
            
            1. Policy层次的检测：一个SR-MPLS TE Policy下面可以配置多条候选路径(Candidate Path)，优先级最高的有效路径是主用Path，优先级次高的有效路径是备份Path。在同一个Candidate Path中多条Segment List可以进行负载分担。多个SR-MPLS TE Policy也可能配置相同的Candidate Path。SR-MPLS TE Policy层次的检测，实际是检测SR-MPLS TE Policy的主用Path和备份Path。  
            2. Candidate Path层次的检测：属于基础的检测，不涉及上层Policy的策略业务，仅仅检测某条Candidate Path是否正常。  

            基于Policy层次的检测，只要找到优选的主备Path，检测方案就等同于Candidate Path层次的检测。主备Path里面如果有多条Segment List，Ping和Tracert会对所有Segment List采用相同的流程进行遍历检测，而且都会生成检测结果。  

            ***SR-MPLS TE Policy Ping***  
            公网PE和P设备都具有SR能力，PE1到PE2方向建立SR-MPLS TE Policy，SR-MPLS TE Policy仅有一个主Path，其Segment List是PE1->P1->p2->PE2。  

            >CE1===PE1===P1===P2===PE2===CE2  

            从PE1节点上发起对该SR-MPLS TE Policy的Ping检测的工作过程如下：  

            1. PE1构造MPLS Echo Request报文，该报文的IP首部目的地址为127.0.0.0/8,源IP地址是MPLS LSR ID，MPLS标签按照SR-MPLS TE Policy下的Segment List进行封装。  
            需要注意的是，在Adjacency SID场景下，头节点PE1感知的FEC仅是P1的，Ping报文到达尾节点PE2后，PE2校验FEC会失败。所以在这种场景下，为了保证尾节点FEC校验能够通过，检测报文需要封装nil_FEC。  
            2. PE1将报文按照SR-MPLS TE Policy的显式路径标签栈逐跳转发给PE2。  
            3. P2将收到报文的最外层标签弹出，然后将报文转发给PE2,此时标签都已弹出，报文将上送主机收发模块进行处理。  
            4. PE2处理报文后，然后构造MPLS Echo Reply报文返回给PE1，其中报文目的地址是PE1的MPLS LSR ID。由于返程报文没有绑定SR-MPLS TE Policy，所以沿着IP路径转发。  
            5. PE1收到回程IP报文后，生成Ping检测结果。  

            如果存在主备Path，且主备Path下有多条Segment List，Ping会对所有Segment List进行遍历检测。  

            ***SR-MPLS TE Policy Tracert***  
            从PE1节点上发起对该SR-MPLS TE Policy的Tracert检测的工作过程如下：  

            1. PE1构造MPLS Echo Request报文，该报文的IP首部目的地址为127.0.0.0/8，源IP地址是MPLS LSR ID，MPLS标签按照SR-MPLS TE Policy下的Segment List进行封装。  
            2. PE1将报文转发给P1，P1判断最外层标签TTL-1是否为0：  
                - TTL-1等于0：MPLS TTL超时上送主机收发模块进行处理。  
                - TTL-1大于0：将最外层MPLS标签弹出，缓存下最外层MPLS标签(TTL-1);把缓存下的最外层MPLS标签(TTl-1)复制到当前的最外层MPLS标签中，查转发表出接口，将报文转发给P2。  
            3. P2的处理同P1，P2判断最外层标签TTL-1是否为0：  
                - TTL-1等于0：MPLS TTL超时上送主机收发模块进行处理。  
                - TTL-1大于0：将最外层MPLS标签弹出，缓存下最外层MPLS标签(TTL-1);把缓存下的最外层MPLS标签(TTL-1)复制到当前的最外层MPLS标签中，查找转发表出接口，将报文转发给PE2。  
            4. 报文转发给PE2,PE2将最外层MPLS标签弹出，报文将上送主机收发模块进行处理，PE2将MPLS Echo Reply报文返回给PE1，其中报文目的地址是PE1的MPLS LSR ID。由于返程报文没有绑定SR-MPLS TE Policy，所以沿着IP路径转发。  
            5. PE1收到回程IP后，生成Tracert检测结果。  
            
            如果存在主备Paht，且主备Path下有多条Segment List，Tracert会对所有Segment List进行遍历检测。  

    + TI-LFA FRR  
        TI-LFA(Topology-Independent Loop-free Alternate)FRR能为Segment Routing隧道提供链路及节点的保护。当某处链路或节点故障时，流量会快速切换到备份路径，继续转发，从而最大程度上避免流量的丢失。  

        SR-MPLS TI-LFA FRR同时适用于SR-MPLS BE和松散形式SR-MPLS TE场景。  

        ***相关概念***  

        |概念|解释|
        |:--:|:--:|
        |P空间|以保护链路源端为根节点建立SPF树，所有从根节点不经过保护链路可达的节点集合称为P空间。|
        |扩展P空间|以保护链路源端的所有邻居为根节点分别建立SPF树，所有从根节点不经过保护链路可达的节点集合称为扩展P空间。P空间或者扩展P空间的节点称为P节点。|
        |Q空间|以保护链路末端为根节点建立反向SPF树，所有从根节点不经过保护链路可达的节点集合称为Q空间。Q空间的节点称为Q节点。|
        |PQ节点|PQ节点是指既在(扩展)P空间又在Q空间的节点，PQ节点会作为保护隧道的目的端。|
        |LFA|LFA(Loop-Free Alternate)算法计算备份链路的基本思路是：以可提供备份链路的邻居为根节点，利用SPF算法计算到达目的节点的最短距离，然后计算出一组开销最小且无环的备份链路。|
        |RLFA|RLFA(Remote LFA)算法根据保护路径计算PQ节点，并在源节点与PQ节点之间建立隧道形成备份下一跳保户。当保护链路发生故障时，流量自动切换到隧道备份路径，继续转发，从而提高网络可靠性。|
        |TI-LFA|LFA FRR和Remote LFA对于某些场景中，扩展P空间和Q空间既没有交集，也没有直连的邻居，无法计算出备份路径，不能满足可靠性要求。在这种情况下，实现了TI-LFA。TI-LFA算法根据保护路径计算扩展P空间，Q空间，Post-convergence最短路径树，以及根据不同场景计算Repair List，并从源节点到P节点，再到Q节点建立Segment Routing隧道形成备份下一跳保护。当保护链路发生故障时，流量自动切换到隧道备份路径，继续转发，从而提高网络可靠性。|

        ***产生原因***  
        传统的LFA技术需要满足至少有一个邻居下一跳到目的节点是无环下一跳。RLFA技术需要满足网络中至少存在一个节点从源节点到该节点，从该节点到目的节点都不经过故障节点。而TI-LFA技术可以用显式路径表达备份路径，对拓扑无约束，提供了更高可靠性的FRR技术。  

        如图所示，现在有数据包需要从DeviceA发往DeviceF。如果扩展P空间与Q空间不相交，则不满足RLFA技术要求，RLFA无法计算出备份路径，也就是Remote LDP LSP。当DeviceB和DeviceE之间发生故障后，DeviceB将数据包转发给DeviceC，但是DeviceC并不是Q节点，无法直接到达目的地址，需要重新计算，由于DeviceC和DeviceD之间开销是100,DeviceC认为到达DeviceF的最优路径是经过DeviceB，因此将数据包重新传回DeviceB，形成环路，转发失败。  

        >DeviceA=1=DeviceB=2=>DeviceC(扩展P空间)
        >============|<====3=====|
        >==========cost10=====cost100
        >============|===========|
        >DeviceF===DeviceE===DeviceD(Q空间)  

        为了解决上述问题，可以使用TI-LFA。当DeviceB和DeivceE之间发生故障后，DeviceB直接启用TI-LFA FRR备份表项，给数据包增加新的路径信息，即“DeviceC的Node SID， DeviceC和DeviceD之间的Adjacency SID”，保证数据包可以沿着备份路径转发。  

        ***TI-LFA FRR原理***  
        PE1为源节点，PE3为目的节点，链路中间的数字表示cost值。假设P1节点为故障点。  
        TI-LFA流量保护分为链路保护和节点保护。  

        1. 链路保护：当需要保护的对象是经过特定链路的流量时，流量保护类型为链路保护。  
        2. 节点保护：当需要保护的对象是经过特定设备的流量时，流量保护类型为节点保护。节点保护优先级高于链路保护。  

        ***TI-LFA FRR转发流程***  
        TI-LFA备份路径计算完成之后，如果主路径发生故障，就可以根据备份路径进行转发，避免流量丢失。  
        
    + Anycast FRR  
        ***Anycast SID***  
        组内所有路由器发布相同的SID，称为Anycast SID。如图，DeviceD和DeviceE都在SR区域出口，通过DeviceD和DeviceE都可以到达非SR区域，因此二者可以互相备份。在这种情况下，可以将DeviceD和DeviceE配置在同一个组里，并且发布相同前缀SID，也即Anycast SID。  
        Anycast SID的下一跳指向路由器中IGP路径开销最小的设备DeviceD，DeviceD也称为发布Anycast SID的最优节点，其他都是备份节点。当到DeviceD的主下一跳链路或直连邻居节点发生故障，可以通过其他保护路径到达Anycast SID设备，Anycast SID设备可以是与下一跳相同的源，也可以是其他Anycast源。VPN使用SR LSP时，Anycast要配置相同的VPN私网标签。  

        ***Anycast FRR***  
        Anycast FRR是指多个节点发布相同的前缀SID，形成了FRR。普通的FRR算法，都只能利用SPT树计算节点的备份下一跳，可以支持路由源头单一的场景，对于多个节点发布相同路由的场景无法支持。  
        多个节点发布相同路由场景下，为了计算前缀SID的备份下一跳，需要将多个路由发布节点变成单个路由发布节点，再进行计算。Anycast FRR方案采用构造一个虚拟节点的方式，将多个相同的路由发布节点转换为单个路由发布节点，然后按TI-LFA算法计算虚拟节点的备份下一跳，Anycast前缀SID从其创建的虚拟节点继承备份下一跳。该方案对备份下一跳计算的算法没有修改，仍然可以保留算法保证的无环特性，算出的备份下一跳和收敛前的周边节点的主下一跳之间不会形成环路。  

    + SR-MPLS防微环  
        IGP协议的链路状态数据库是分布式的，这样IGP在无序收敛时可能会产生环路。但这种环路会在转发路径上所有设备都完成收敛之后消失，这种暂态的环路被称为微环路(Micro Loop)。微环可能导致网络丢包，时延抖动和报文乱序等一系列问题，所以必须予以重视。  
        Segment Routing采用一种对网络影响比较小的方式来消除网络中潜在的环路。它的原理是：如果网络拓扑变化可能引发环路，网络节点通过创建一个无环的Segment List，引导流量转发到目的地址，等待网络节点全部完成收敛以后再回退到正常转发状态，从而能有效地消除网络中的环路。  

        ***SR-MPLS 本地正切防微环***  
        本地正切防微环指的是紧邻故障节点的节点收敛后引发的环路。如图所示，全网节点都部署TI-LFA，当节点B故障的时候，节点A针对目的地址C的收敛过程如下：  

        1. 节点A感知到故障，进入TI-LFA的快速重路由切换流程，向报文插入Repair List<16056>，将报文转向TI-LFA计算的PQ节点E。因此报文会先转发到下一跳节点D，此时Segment List中的SID为：<16056,103>。  
        2. 当节点A完成到目的地址C的路由收敛，则直接查找节点C的路由，将报文转发到下一跳节点D，此时不再携带Repair List，而是直接基于Node SID 103进行转发。  
        3. 如果此时节点D还未完成收敛，当节点A向节点D转发报文时，节点D的转发表中到节点C的路由下一跳还是节点A，这样就在节点A和节点D之间形成了环路。  

        >A(SID 101)===B(SID 102)===C(SID 103)
        >payload+103+16056(A收敛后，退出TI-LFA后，此SID信息被去掉)|
        >D(SID 104)===E(SID 105)===F(SID 106)

        通过上述收敛过程的描述可以看出，本地正切环路发生在节点A完成收敛，退出TI-LFA过程，变成正常转发，而网络中其他节点还未完成收敛期间。因此解决这个场景下的环路问题，只需要节点A延时收敛。因为TI-LFA一定是个无环路径，所以只需要维持一段时间按照TI-LFA路径转发，待网络中其他接点完成收敛以后再退出TI-LFA，正常收敛，即可避免正切微环。  

        部署正切防微环后的收敛流程如下：  

        1. 节点A感知到故障，进入TI-LFA流程，报文沿着备份路径转发，下一跳为节点D，并封装Repair List<16056>。  
        2. 节点A启动一个定时器T1。在T1期间，节点A不响应拓扑变化，转发表不变，报文依旧按照TI-LFA策略转发。网络中其他节点正常收敛。  
        3. 节点A的定时器T1超时，这时网络中其他节点都已经完成收敛，节点A也正常收敛，退出TI-LFA流程，按照正常收敛后的路径转发报文。  

        通过上述步骤能够有效地避免PLR节点的正切微环问题，但是需要注意，该方案之能解决PLR接点的正切微环问题。这是因为只有在PLR节点才能进入TI-LFA转发流程，才可以通过延时收敛，继续使用TI-LFA路径实现防微环。此外，该方案只限于单点故障，如果是多点故障，TI-LFA备份路径可能也会受影响。  
        在源节点上配置正切防微环功能，路由延时切换需要满足以下条件：  

        - 本地直连接口故障/BFD down。  
        - 在延迟期间，网络中没有第二次拓扑变化。  
        - 路由有备份下一跳。  
        - 路由主下一跳和故障端口相同。  
        - 多节点路由延时期间收到路由发布源头变化退出延时。  

        ***SR-MPLS 本地回切防微环***  
        微环不但可能在路径正切时产生，也可能在故障恢复后路径回切时出现。如图所示：  

        1. 节点A将报文按照路径A->B->C->E->F发送到目的节点F。当B-C之间的链路发生故障之后，A节点会将报文按照重新收敛之后的路径A->B->D->E->F发送到目的节点F。  
        2. 节点B和节点C之间的链路故障恢复后，假设节点D率先完成收敛。  
        3. 节点A将报文发给节点B，由于节点B未完成收敛，依然按照故障恢复前路径转发，转发给节点D。  
        4. 节点D已经完成收敛，所以节点D按照故障恢复后的路径转发到节点B，这样就在节点B和节点D之间形成了环路。  
        
        由于回切不会进入TI-LFA转发流程，所以无法像正切一样使用延时收敛的方式解决回切微环问题。  
        >A===B===C====
        >====|===|====
        >====D===E===F

        从上述回切微环产生过程中可以看出，当故障恢复的时候，节点D先于节点B收敛会产生短暂的环路。由于节点D无法预估网络中的链路UP事件，所以也无法预先安装针对链路UP事件计算的无环路径。为了消除回切过程中潜在的环路问题，节点D需要能够收敛到一条无环路径。  
        如图，节点D感知到B-C的链路UP事件以后，重新收敛到D->B->C->E->F路径。  
        此外，B-C链路UP事件不会影响到D-B的路径，所以节点D到B的路径一定是一个无环路径。  
        因此，在构造节点D到节点F的无环路径的时候，无须指定节点D到节点B的路径。同理，节点C到节点F也不会受B-C链路UP事件的影响。节点C到F的路径也一定是无环路径。唯一受影响的是节点B到节点C的路径，所以要计算节点D到节点F的无环路径，只需要指定节点B到节点C的路径，所以要计算节点D到节点F的无环路径，只需要节点B到节点C的路经，所以要计算节点D到节点F的无环路径，只需要指定节点B到节点C的路径即可。根据上述分析，只需要在节点D收敛后的路径中插入一个节点B到节点C的Adjacency SID指示报文从节点B转发到节点C，就可以保证节点D到节点F的路径无环。  
        部署回切防微环后的收敛流程如下：  

        1. 节点B和节点C的链路故障后恢复，节点D率先完成收敛。  
        2. 节点D启动定时器T1，在T1超时前，节点D针对访问节点F的报文计算的防微环Segment List为<16023>。  
        3. 节点A将报文转发给节点B，由于节点B未完成收敛，依然按照故障恢复前路径转发，转发给节点D。  
        4. 节点D在报文中插入防微环Segment List<16023>，并转发到节点B。  
        5. 节点B根据Adjacency SID 16023指令执行转发动作，沿着Adjacency SID 16023指定的出接口转发到节点C，并将外层标签16023去除。  
        6. 节点C根据Node SID 106按最短路径转发到目的地址F。  

        从以上过程可以看出，由于节点D转发报文的时候插入了防微环Segment List<16023>，所以消除了网络中潜在的环路。  
        当节点D的定时器T1超时后，网络中的其他节点也都已经完成收敛，头节点A重新按照收敛后的正常路径A->B->C->E->F转发报文。  

        ***SR-MPLS远端正切防微环***  
        前面介绍了本地正切防微环，实际上正切时不仅会导致本地微环，也可能引起远端节点之间形成环路，即沿着报文转发路径，如果离故障点更近的节点先于离故障点远的节点收敛，就可能会导致环路。下图，描述远端正切微环产生过程：  

        1. 节点C和节点E之间的链路故障，假设节点G率先完成收敛，节点B未完成收敛。  
        2. 节点A和节点B沿着故障前路径将报文转发到节点G。  
        3. 由于节点G已经完成收敛，根据路由下一跳转发到节点B。这样报文就在节点B和节点G之间形成了环路。  

        >A===B===G===C====  
        >====|=======|(断点)
        >====D=======E===F

        由于计算量的关系，通常网络节点只能针对本地直连的链路或节点故障预先计算无环路径，而无法针对网络中任何其他潜在的故障预先计算无环路径。因此要解决此场景的微环问题，只能在节点G收敛以后安装一条无环路径。  

        上面已经提到，链路Down触发的拓扑变化只会影响收敛前经过该链路的转发路径，如果收敛前某节点到目的节点的路径不经过故障链路，那该路径一定不会受该链路故障事件的影响。从图中拓扑可以看出，节点G到D的路径不会受到C-E的链路故障的影响，所以节点G到F的无环路径不需要指定节点G到D的路径不会受C-E的链路故障的影响，所以节点G到F的无环路径不需要指定节点G到节点D这段路径。同样，节点E到节点F也不会受C-E的链路故障的影响，所以也无须指定节点E到F的路径。同样，节点E到节点F也不会受C->E的链路故障的影响，所以也无须指定节点E到F的路径。这样一来，只有节点D到节点E的路径受C-E的链路故障的影响，所以无环路径只需要指定节点D到节点E的Adjacency SID 16045即可。
        使能远端正切防微环后的收敛流程如下：  

        1. 节点C和节点E之间的链路故障，假设节点G率先完成收敛。  
        2. 节点G启动定时器T1，在T1超时前，节点G针对访问节点F的报文计算的防微环Segment List为<16045>。  
        3. 节点A将报文转发给节点B，由于节点B未完成收敛，依然按照故障发生之前的路径将报文转发给节点G。  
        4. 节点G在报文中插入防微环Segment List<16045>，并转发到节点B。  
        5. 节点B根据Adjacency SID 16045的指令执行转发动作，将报文转发给节点D。  
        6. 节点D根据Adjacency SID 16045的指令执行转发动作，沿着Adjacency SID 16045指定的出接口转发到节点E，并将外层标签16045去除。  
        7. 节点E根据Node SID 106将报文按最短路径转发到目的地址F。  

        从以上过程可以看出，由于节点G转发报文的时候插入了防微环Segment List<16045>，所以消除了网络中潜在的环路。  
        当节点G的定时器T1超时后，网络中的其他节点也都已经完成了收敛，头节点A按照正常收敛后的路径A->B->D->E->F转发报文。  

        ***SR-MPLS远端回切防微环***  
        其过程如下：  

        1. 节点C和节点E之间的链路故障恢复，假设节点B率先完成收敛，节点G未完成收敛。  
        2. 节点B将报文转发给节点G。  
        3. 但是节点G未完成收敛，仍旧按照故障恢复前路径将报文转发给节点B。这样报文在节点B和节点G之间形成了环路。  

        解决思路就是在节点B启动定时器T1，在T1超时前，节点B针对访问节点F的报文，增加节点G和节点C之间的Adjacency SID，确保报文转发到节点C。这样节点C就可以根据Node SID 106按最短路径转发到目的地址F。  

    + SR-MPLS OAM  
        SR-MPLS OAM
    + MPLS in UDP
    + SR-MPLS TTL  

3. Segment Routing MPLS 应用  
4. Segment Routing MPLS 术语与缩略语

#### 1.2 Segment Routing MPLS配置

### 2. Segment Routing IPv6  

#### 2.1 Segment Routing IPv6特性描述  

#### 2.2 Segment Routing IPv6配置
