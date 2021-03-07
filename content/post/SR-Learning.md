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
