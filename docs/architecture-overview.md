# Architecture Overview - Virtual Switch Network

## System Architecture

```mermaid
graph TB
    subgraph Host1["Host 1"]
        App1[Application 1]
        Kernel1[Linux Network Stack]
        TAP1[TAP Device: tapyuan]
        VPort1[VPort Process]
        
        App1 <--> Kernel1
        Kernel1 <--> TAP1
        TAP1 <--> VPort1
    end
    
    subgraph Host2["Host 2"]
        App2[Application 2]
        Kernel2[Linux Network Stack]
        TAP2[TAP Device: tapyuan]
        VPort2[VPort Process]
        
        App2 <--> Kernel2
        Kernel2 <--> TAP2
        TAP2 <--> VPort2
    end
    
    subgraph Host3["Host 3"]
        App3[Application 3]
        Kernel3[Linux Network Stack]
        TAP3[TAP Device: tapyuan]
        VPort3[VPort Process]
        
        App3 <--> Kernel3
        Kernel3 <--> TAP3
        TAP3 <--> VPort3
    end
    
    subgraph Server["VSwitch Server"]
        VSwitch[VSwitch Python Process]
        MACTable[(MAC Address Table)]
        
        VSwitch <--> MACTable
    end
    
    VPort1 <-->|UDP Socket| VSwitch
    VPort2 <-->|UDP Socket| VSwitch
    VPort3 <-->|UDP Socket| VSwitch
    
    style VSwitch fill:#f9f,stroke:#333,stroke-width:4px
    style MACTable fill:#bbf,stroke:#333,stroke-width:2px
    style TAP1 fill:#bfb,stroke:#333,stroke-width:2px
    style TAP2 fill:#bfb,stroke:#333,stroke-width:2px
    style TAP3 fill:#bfb,stroke:#333,stroke-width:2px
```

## Component Interaction Diagram

```mermaid
graph LR
    subgraph Utils["Utility Modules"]
        SysUtils[sys_utils.h<br/>Error Handling]
        TAPUtils[tap_utils.h/c<br/>TAP Device Management]
    end
    
    subgraph VPortComp["VPort Component (C)"]
        VPortStruct[vport_t struct<br/>- tapfd<br/>- vport_sockfd<br/>- vswitch_addr]
        UpThread[Up Forwarder Thread<br/>TAP → VSwitch]
        DownThread[Down Forwarder Thread<br/>VSwitch → TAP]
        
        VPortStruct --> UpThread
        VPortStruct --> DownThread
    end
    
    subgraph VSwitchComp["VSwitch Component (Python)"]
        UDPServer[UDP Server Socket]
        MACLearning[MAC Learning Logic]
        Forwarding[Frame Forwarding Logic]
        
        UDPServer --> MACLearning
        MACLearning --> Forwarding
    end
    
    TAPUtils --> VPortStruct
    SysUtils --> VPortStruct
    SysUtils --> TAPUtils
    
    UpThread <-->|Ethernet Frames| UDPServer
    DownThread <-->|Ethernet Frames| UDPServer
    
    style VPortComp fill:#fdd,stroke:#333,stroke-width:2px
    style VSwitchComp fill:#ddf,stroke:#333,stroke-width:2px
    style Utils fill:#dfd,stroke:#333,stroke-width:2px
```

## Data Flow Architecture

```mermaid
flowchart TD
    Start([Application sends packet])
    
    Start --> A[Packet enters Linux network stack]
    A --> B{Routing decision}
    B --> C[Route to TAP interface]
    
    C --> D[VPort reads from TAP device<br/>via read system call]
    D --> E[Parse Ethernet frame<br/>Extract: src MAC, dst MAC, type]
    
    E --> F[Encapsulate in UDP packet]
    F --> G[Send to VSwitch via UDP socket]
    
    G --> H[VSwitch receives UDP packet]
    H --> I[Extract Ethernet frame]
    I --> J[Parse frame headers]
    
    J --> K{Check MAC table}
    K -->|Source MAC not in table| L[Learn: Add/Update MAC table<br/>map src MAC to sender VPort]
    K -->|Source MAC in table| M[Continue]
    L --> M
    
    M --> N{Determine forwarding}
    N -->|Unicast & in table| O[Forward to specific VPort]
    N -->|Broadcast MAC| P[Forward to all VPorts except source]
    N -->|Unknown MAC| Q[Discard frame]
    
    O --> R[Destination VPort receives UDP]
    P --> R
    
    R --> S[Extract Ethernet frame]
    S --> T[Write to TAP device]
    T --> U[Linux network stack processes frame]
    U --> V([Deliver to application])
    
    Q --> End([End])
    V --> End
    
    style Start fill:#bfb,stroke:#333,stroke-width:2px
    style End fill:#fbb,stroke:#333,stroke-width:2px
    style H fill:#bbf,stroke:#333,stroke-width:3px
    style L fill:#fbf,stroke:#333,stroke-width:2px
```

## Threading Architecture

```mermaid
graph TD
    subgraph MainThread["Main Thread"]
        Init[Initialize VPort<br/>- Create TAP device<br/>- Create UDP socket<br/>- Setup VSwitch address]
        CreateThreads[Create worker threads]
        Wait[pthread_join<br/>Wait for threads]
    end
    
    subgraph UpForwarder["Up Forwarder Thread"]
        UpLoop[Infinite Loop]
        ReadTAP[read from TAP device]
        ParseUp[Parse Ethernet header]
        SendUDP[sendto VSwitch]
        LogUp[Log frame details]
        
        UpLoop --> ReadTAP
        ReadTAP --> ParseUp
        ParseUp --> SendUDP
        SendUDP --> LogUp
        LogUp --> UpLoop
    end
    
    subgraph DownForwarder["Down Forwarder Thread"]
        DownLoop[Infinite Loop]
        RecvUDP[recvfrom VSwitch]
        ParseDown[Parse Ethernet header]
        WriteTAP[write to TAP device]
        LogDown[Log frame details]
        
        DownLoop --> RecvUDP
        RecvUDP --> ParseDown
        ParseDown --> WriteTAP
        WriteTAP --> LogDown
        LogDown --> DownLoop
    end
    
    Init --> CreateThreads
    CreateThreads --> UpForwarder
    CreateThreads --> DownForwarder
    CreateThreads --> Wait
    
    style MainThread fill:#fdd,stroke:#333,stroke-width:2px
    style UpForwarder fill:#dfd,stroke:#333,stroke-width:2px
    style DownForwarder fill:#ddf,stroke:#333,stroke-width:2px
```

## MAC Learning State Machine

```mermaid
stateDiagram-v2
    [*] --> Idle: VSwitch starts
    
    Idle --> FrameReceived: Receive ethernet frame<br/>from VPort
    
    FrameReceived --> ParseFrame: Extract headers
    ParseFrame --> CheckMAC: Get source MAC
    
    CheckMAC --> MACNotInTable: MAC not in table
    CheckMAC --> MACInTable: MAC in table
    
    MACNotInTable --> LearnMAC: mac_table[src_mac] = vport_addr
    MACInTable --> CheckAddr: Compare VPort address
    
    CheckAddr --> UpdateMAC: Address changed
    CheckAddr --> ForwardingDecision: Address same
    
    LearnMAC --> LogUpdate: Log ARP cache update
    UpdateMAC --> LogUpdate
    LogUpdate --> ForwardingDecision
    
    ForwardingDecision --> UnicastKnown: dst_mac in table
    ForwardingDecision --> Broadcast: dst_mac is ff:ff:ff:ff:ff:ff
    ForwardingDecision --> Discard: Unknown destination
    
    UnicastKnown --> SendToVPort: sendto(specific VPort)
    Broadcast --> SendToAll: sendto(all VPorts except src)
    
    SendToVPort --> Idle
    SendToAll --> Idle
    Discard --> Idle
```

## Key Design Decisions

### 1. TAP Device (Layer 2)
- **Why TAP?**: Operates at Ethernet layer, allows full control over frames
- **Alternative TUN**: Would be Layer 3 (IP), less flexible for virtual networking

### 2. UDP for Transport
- **Advantages**: 
  - Low latency
  - No connection state
  - Simple broadcast/multicast support
- **Trade-offs**: No reliability guarantees (acceptable for local network simulation)

### 3. Learning Switch Algorithm
- **Dynamic MAC learning**: Adapts to network topology changes
- **Broadcast handling**: Enables ARP and other discovery protocols
- **Discard unknown**: Prevents unnecessary traffic (could implement flooding)

### 4. Threading Model
- **Separate threads for up/down**: Maximizes throughput, prevents blocking
- **Continuous polling**: Low latency at cost of CPU usage
- **Alternative**: Could use epoll/select for efficiency

### 5. Frame Processing
- **IFF_NO_PI flag**: Disables packet information header for simpler processing
- **ETHER_MAX_LEN buffer**: 1518 bytes (max standard Ethernet frame)
- **Minimum frame check**: Assert 14 bytes (Ethernet header size)

## Performance Considerations

### Latency Sources
1. **TAP device read/write**: Kernel → userspace transition
2. **UDP socket operations**: Network stack overhead
3. **Context switching**: Between threads and processes
4. **MAC table lookup**: O(1) dictionary access in Python

### Scalability
- **VSwitch bottleneck**: Single-threaded Python process
- **VPort parallelism**: Each runs independently
- **Network bandwidth**: Limited by UDP socket buffer sizes

### Optimization Opportunities
1. Use multi-threaded VSwitch
2. Implement zero-copy packet forwarding
3. Use hardware offloading (if available)
4. Optimize MAC table with C/C++ implementation
5. Consider using DPDK for high-performance packet processing

