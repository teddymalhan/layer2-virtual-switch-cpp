# Sequence Diagrams - VPN Virtual Switch Operations

## 1. VPort Initialization Sequence

```mermaid
sequenceDiagram
    participant Main as VPort Main
    participant VPort as vport_t
    participant TAPUtils as tap_utils
    participant Kernel as Linux Kernel
    participant Socket as UDP Socket
    participant VSwitch as VSwitch
    
    Main->>Main: Parse command line args<br/>(server_ip, server_port)
    Main->>VPort: vport_init(&vport, ip, port)
    
    VPort->>TAPUtils: tap_alloc("tapyuan")
    TAPUtils->>Kernel: open("/dev/net/tun", O_RDWR)
    Kernel-->>TAPUtils: fd
    TAPUtils->>Kernel: ioctl(fd, TUNSETIFF, IFF_TAP)
    Kernel-->>TAPUtils: Configure TAP device
    TAPUtils-->>VPort: tapfd
    
    VPort->>Socket: socket(AF_INET, SOCK_DGRAM, 0)
    Socket-->>VPort: vport_sockfd
    
    VPort->>VPort: Setup vswitch_addr<br/>(inet_pton)
    VPort-->>Main: Initialized vport
    
    Main->>Main: pthread_create(up_forwarder)
    Main->>Main: pthread_create(down_forwarder)
    Main->>Main: pthread_join() both threads
```

## 2. Ethernet Frame Forwarding: TAP → VSwitch

```mermaid
sequenceDiagram
    participant App as Linux Application
    participant Kernel as Linux Kernel
    participant TAP as TAP Device
    participant VPort as VPort Thread
    participant Socket as UDP Socket
    participant VSwitch as VSwitch
    
    App->>Kernel: Send packet to tapyuan interface
    Kernel->>TAP: Route to TAP device
    
    loop Continuous forwarding
        VPort->>TAP: read(tapfd, buffer, size)
        TAP-->>VPort: Ethernet frame (14+ bytes)
        
        VPort->>VPort: Parse ethernet header<br/>(src MAC, dst MAC, type)
        
        VPort->>Socket: sendto(vport_sockfd, frame, size,<br/>vswitch_addr)
        Socket->>VSwitch: UDP packet with ethernet frame
        
        VPort->>VPort: Log frame details
    end
```

## 3. VSwitch Frame Processing and Forwarding

```mermaid
sequenceDiagram
    participant VPort1 as VPort 1
    participant VSwitch as VSwitch
    participant MACTable as MAC Table
    participant VPort2 as VPort 2
    participant VPort3 as VPort 3
    
    VPort1->>VSwitch: UDP packet (ethernet frame)
    VSwitch->>VSwitch: recvfrom() → data, vport_addr
    
    VSwitch->>VSwitch: Parse ethernet header<br/>(extract src & dst MAC)
    
    VSwitch->>MACTable: Check if src_mac exists
    alt MAC not in table or address changed
        VSwitch->>MACTable: mac_table[src_mac] = vport_addr
        VSwitch->>VSwitch: Log: "ARP Cache updated"
    end
    
    alt Destination is in MAC table
        VSwitch->>MACTable: dst_addr = mac_table[dst_mac]
        VSwitch->>VPort2: sendto(data, dst_addr)
        VSwitch->>VSwitch: Log: "Forwarded to dst_mac"
    else Destination is broadcast (ff:ff:ff:ff:ff:ff)
        VSwitch->>MACTable: Get all MACs except src
        loop For each destination VPort
            VSwitch->>VPort2: sendto(data, vport2_addr)
            VSwitch->>VPort3: sendto(data, vport3_addr)
        end
        VSwitch->>VSwitch: Log: "Broadcasted"
    else Unknown destination
        VSwitch->>VSwitch: Discard frame
        VSwitch->>VSwitch: Log: "Discarded"
    end
```

## 4. Ethernet Frame Forwarding: VSwitch → TAP

```mermaid
sequenceDiagram
    participant VSwitch as VSwitch
    participant Socket as UDP Socket
    participant VPort as VPort Thread
    participant TAP as TAP Device
    participant Kernel as Linux Kernel
    participant App as Linux Application
    
    VSwitch->>Socket: sendto(frame, vport_addr)
    
    loop Continuous forwarding
        VPort->>Socket: recvfrom(vport_sockfd, buffer)
        Socket-->>VPort: Ethernet frame from VSwitch
        
        VPort->>VPort: Parse ethernet header<br/>(src MAC, dst MAC, type)
        
        VPort->>TAP: write(tapfd, frame, size)
        TAP->>Kernel: Inject frame into network stack
        
        Kernel->>Kernel: Process ethernet frame
        Kernel->>App: Deliver to application
        
        VPort->>VPort: Log frame details
    end
```

## 5. Complete End-to-End Communication Flow

```mermaid
sequenceDiagram
    participant App1 as Application 1
    participant VPort1 as VPort 1<br/>(TAP: tapyuan)
    participant VSwitch as VSwitch<br/>(UDP Server)
    participant VPort2 as VPort 2<br/>(TAP: tapyuan)
    participant App2 as Application 2
    
    Note over App1,App2: Setup Phase
    VPort1->>VSwitch: Connect & register
    VPort2->>VSwitch: Connect & register
    
    Note over App1,App2: Communication Phase
    App1->>VPort1: Send data to peer MAC address
    
    VPort1->>VPort1: Read from TAP device
    VPort1->>VSwitch: Forward ethernet frame (UDP)
    
    VSwitch->>VSwitch: Update MAC table<br/>(MAC1 → VPort1 addr)
    VSwitch->>VSwitch: Lookup destination MAC2
    VSwitch->>VPort2: Forward to VPort2 (UDP)
    
    VPort2->>VPort2: Receive from VSwitch
    VPort2->>VPort2: Write to TAP device
    
    VPort2->>App2: Deliver to Application 2
    
    Note over App1,App2: Response Path
    App2->>VPort2: Send response
    VPort2->>VSwitch: Forward ethernet frame
    VSwitch->>VSwitch: Update MAC table<br/>(MAC2 → VPort2 addr)
    VSwitch->>VPort1: Forward to VPort1
    VPort1->>App1: Deliver response
```

## 6. Multi-VPort Broadcast Scenario

```mermaid
sequenceDiagram
    participant VPort1 as VPort 1
    participant VSwitch as VSwitch
    participant VPort2 as VPort 2
    participant VPort3 as VPort 3
    participant VPort4 as VPort 4
    
    Note over VPort1,VPort4: ARP Request (Broadcast)
    
    VPort1->>VSwitch: Ethernet frame<br/>(dst: ff:ff:ff:ff:ff:ff)
    
    VSwitch->>VSwitch: Learn: MAC1 → VPort1
    VSwitch->>VSwitch: Detect broadcast destination
    VSwitch->>VSwitch: Get all VPorts except VPort1
    
    par Broadcast to all VPorts
        VSwitch->>VPort2: Forward frame
        VSwitch->>VPort3: Forward frame
        VSwitch->>VPort4: Forward frame
    end
    
    Note over VPort1,VPort4: ARP Reply (Unicast)
    
    VPort3->>VSwitch: ARP reply<br/>(dst: MAC1)
    VSwitch->>VSwitch: Learn: MAC3 → VPort3
    VSwitch->>VSwitch: Lookup MAC1 in table
    VSwitch->>VPort1: Forward to VPort1 only
```

## Operational Notes

### Threading Model
- **VPort**: Uses 2 threads per instance
  - Thread 1: Continuous read from TAP → forward to VSwitch
  - Thread 2: Continuous receive from VSwitch → write to TAP

### Error Handling
- Fatal errors use `ERROR_PRINT_THEN_EXIT` macro
- Size mismatches are logged but don't terminate execution
- Assertions verify ethernet frame minimum size (14 bytes)

### Performance Characteristics
- **UDP**: Low latency, no connection overhead
- **Zero-copy where possible**: Direct buffer forwarding
- **Non-blocking**: Continuous polling for frames
- **Learning Switch**: Dynamic MAC table for efficient forwarding

