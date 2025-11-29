# Modern C++ Virtual Switch Network

This project implements a Layer-2 Virtual Switch that forwards ethernet frames over UDP.  
It connects TAP devices across machines and behaves like a simple switch.

This started as a systems and networking learning project focused on clean C++ design, concurrency, and correctness.

## What it does

- Bridges TAP devices over UDP  
- Learns MAC addresses dynamically  
- Forwards unicast frames and broadcasts unknown destinations  
- Works on Linux and macOS  
- Uses RAII for resource management and thread safety  

Main components are:

- **VSwitch**. Central switch that learns MACs and forwards frames.  
- **VPort**. Client that connects a local TAP interface to the switch.  
- **MacTable**. Thread-safe MAC address table.

# Build and Run

## Option 1: Docker

```bash
# Build and run the automated test
docker build -t vpn-alpine -f tests/Dockerfile .
chmod +x tests/test_in_docker.sh
./tests/test_in_docker.sh
```
This will automatically:

- Start VSwitch in a container
- Start two VPort clients
- Configure TAP devices
- Test connectivity
- Show detailed logs

# Usage (how it works behinds the scenes on Linux)

```bash
# Terminal 1: Start VSwitch
./build/vswitch 8080

# Terminal 2: Start VPort 1
sudo ./build/vport 127.0.0.1 8080

# Terminal 3: Start VPort 2
sudo ./build/vport 127.0.0.1 8080
```

# Configure TAP Devices

```bash
# Get device names from VPort output on Linux
sudo ip addr add 10.1.1.101/24 dev tap0
sudo ip link set tap0 up
```

# Test Connectivity

```bash
# Ping between VPorts
ping 10.1.1.102
# You should see:
# - VSwitch learning MAC addresses
# - Frame forwarding logs
# - Successful connectivity
```
