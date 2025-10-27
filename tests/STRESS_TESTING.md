# Stress Testing Guide

## Overview

Stress testing the VPN/Virtual Switch system to verify stability, performance, and resource management under load.

## Test Categories

1. **Throughput Tests** - High packet rate processing
2. **Scale Tests** - Multiple VPort connections
3. **Memory Tests** - Long-running memory leak detection
4. **Frame Size Tests** - Large Ethernet frames
5. **Concurrency Tests** - Thread safety under load

---

## Prerequisites

```bash
# Build with optimizations
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Build with debug symbols for memory testing
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

---

## 1. Throughput Tests

### High-Rate Packet Generation

```bash
# Terminal 1: Start VSwitch
./build/vswitch 8080

# Terminal 2: Start VPort 1
sudo ./build/vport 127.0.0.1 8080 tap0

# Terminal 3: Start VPort 2
sudo ./build/vport 127.0.0.1 8080 tap1

# Terminal 4: Configure TAP devices
sudo ip addr add 10.1.1.101/24 dev tap0
sudo ip addr add 10.1.1.102/24 dev tap1
sudo ip link set tap0 up
sudo ip link set tap1 up

# Terminal 5: High-rate ping test
ping -i 0.01 -c 10000 10.1.1.102  # 100 pings per second for 10k packets

# Or use flood ping
ping -f 10.1.1.102  # Flood ping (super high rate)
```

### Expected Results

- **Packet Loss**: < 0.1%
- **Latency**: < 5ms
- **CPU Usage**: < 80%
- **No crashes or memory leaks**

---

## 2. Scale Tests

### Multiple VPort Connections

Test with multiple VPorts connected to one VSwitch:

```bash
# Terminal 1: Start VSwitch
./build/vswitch 8080

# Start multiple VPorts (terminals 2-N)
sudo ./build/vport 127.0.0.1 8080 tap0
sudo ./build/vport 127.0.0.1 8080 tap1
sudo ./build/vport 127.0.0.1 8080 tap2
sudo ./build/vport 127.0.0.1 8080 tap3
# ... up to 10 VPorts
```

### Automated Scale Test Script

```bash
#!/bin/bash
# tests/stress_scale_test.sh

echo "Starting Scale Test..."

# Start VSwitch
./build/vswitch 8080 &
VSWITCH_PID=$!
sleep 2

# Start multiple VPorts
for i in {0..9}; do
  sudo ./build/vport 127.0.0.1 8080 tap$i &
  sleep 0.5
  sudo ip addr add 10.1.1.10$i/24 dev tap$i
  sudo ip link set tap$i up
done

echo "Scale test running... Press Ctrl+C to stop"
wait $VSWITCH_PID
```

### Expected Results

- **Max VPorts**: 10+ concurrent connections
- **MAC Table**: All MACs learned correctly
- **No frame loss** between any pair
- **CPU Usage**: Scales linearly

---

## 3. Memory Leak Detection

### Using Valgrind

```bash
# Build debug version
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug

# Test VPort memory leaks
valgrind --leak-check=full --track-origins=yes \
  sudo ./build-debug/vport 127.0.0.1 8080 tap0

# Test VSwitch memory leaks
valgrind --leak-check=full --track-origins=yes \
  ./build-debug/vswitch 8080
```

### Long-Running Memory Test

```bash
# Start VSwitch with memory monitoring
valgrind --leak-check=full ./build-debug/vswitch 8080 &
VSWITCH_PID=$!

# Start two VPorts
sudo valgrind --leak-check=full ./build-debug/vport 127.0.0.1 8080 tap0 &
VPORT1_PID=$!

sudo valgrind --leak-check=full ./build-debug/vport 127.0.0.1 8080 tap1 &
VPORT2_PID=$!

# Configure TAP devices
sudo ip addr add 10.1.1.101/24 dev tap0
sudo ip addr add 10.1.1.102/24 dev tap1
sudo ip link set tap0 up
sudo ip link set tap1 up

# Generate continuous traffic for 1 hour
ping -i 1 -c 3600 10.1.1.102

# Check for leaks
kill $VSWITCH_PID $VPORT1_PID $VPORT2_PID
```

### Expected Results

- **No definitely lost blocks** in Valgrind
- **Memory usage**: Stable over time
- **No growing memory** during extended runs

---

## 4. Large Frame Tests

### Test with Maximum Ethernet Frame Size

```bash
#!/bin/bash
# tests/stress_large_frames.sh

# Install requirements
sudo apt-get install -y scapy  # or use Python scapy

# Create large payload (1500 bytes minus headers)
python3 << 'EOF'
from scapy.all import *
import socket

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Large payload
payload = b'A' * 1448  # Max ethernet payload

# Send large frames continuously
for i in range(1000):
    eth = Ether(dst="ff:ff:ff:ff:ff:ff", src="00:11:22:33:44:55")
    ip = IP(dst="10.1.1.102")
    udp = UDP()
    data = Raw(load=payload)
    frame = eth/ip/udp/data
    sock.sendto(bytes(frame), ("127.0.0.1", 8080))

sock.close()
EOF
```

### Expected Results

- **1500-byte frames**: Processed correctly
- **No frame drops** due to size
- **Correct forwarding** of large frames

---

## 5. Concurrent Operations Test

### Simultaneous Read/Write

```bash
#!/bin/bash
# tests/stress_concurrency.sh

# Create multiple ping processes
for i in {1..10}; do
  (while true; do ping -c 1 10.1.1.102 > /dev/null; sleep 0.1; done) &
done

# Let run for 5 minutes
sleep 300

# Kill all background processes
killall ping
```

### Expected Results

- **Thread-safety**: No crashes
- **Correct frame ordering**: Ordered delivery
- **No deadlocks**: System remains responsive

---

## 6. Combined Stress Test Script

```bash
#!/bin/bash
# tests/stress_comprehensive.sh

set -e

echo "=== Comprehensive Stress Test ==="
echo "Duration: 10 minutes"
echo "VPorts: 4"
echo "Packet Rate: 100 pps per VPort"

# Start VSwitch
./build/vswitch 8080 &
VSWITCH_PID=$!
echo "VSwitch PID: $VSWITCH_PID"
sleep 2

# Start 4 VPorts
for i in {0..3}; do
  sudo ./build/vport 127.0.0.1 8080 tap$i &
  VPORT_PIDS[$i]=$!
  sleep 0.5
  
  sudo ip addr add 10.1.1.10$i/24 dev tap$i
  sudo ip link set tap$i up
  echo "VPort $i started, TAP tap$i configured"
done

echo "All components started"

# Generate traffic between all pairs
generate_traffic() {
  local src=$1
  local dst=$2
  
  for i in {1..6000}; do  # 10 minutes at 10 pps
    ping -c 1 -i 0.1 10.1.1.10$dst > /dev/null 2>&1
  done
}

# Start traffic generators
for src in {0..3}; do
  for dst in {0..3}; do
    if [ $src -ne $dst ]; then
      generate_traffic $src $dst &
    fi
  done
done

echo "Traffic generators started"
sleep 600  # Run for 10 minutes

# Cleanup
echo "Stopping stress test..."
kill $VSWITCH_PID
for pid in "${VPORT_PIDS[@]}"; do
  kill $pid
done

echo "Stress test complete!"
```

---

## Monitoring During Tests

### System Resources

```bash
# Monitor CPU and memory
top -b -n 1 | grep -E "(PID|vswitch|vport)"

# Monitor network
watch -n 1 "ifconfig | grep -A 5 tap"

# Monitor UDP sockets
watch -n 1 "netstat -un | grep 8080"
```

### VSwitch Performance

```bash
# Watch VSwitch logs for:
# - Frame reception rate
# - MAC learning rate
# - Forwarding rate
# - Error messages

tail -f logs/vswitch.log
```

---

## Performance Benchmarks

Expected performance metrics:

| Metric | Target | Measurement |
|--------|--------|-------------|
| **Latency** | < 5ms | ping times |
| **Throughput** | > 100 Mbps | packet capture |
| **Max Connections** | > 10 VPorts | concurrent connections |
| **Packet Loss** | < 0.1% | throughput test |
| **Memory Usage** | < 100MB | valgrind / top |
| **CPU Usage** | < 80% under load | top |
| **Frame Size** | Up to 1500 bytes | large frame test |

---

## Automated Stress Test Suite

Run all stress tests:

```bash
chmod +x tests/stress_*.sh

# Individual tests
./tests/stress_scale_test.sh
./tests/stress_large_frames.sh
./tests/stress_concurrency.sh
./tests/stress_comprehensive.sh

# Or run all
for test in tests/stress_*.sh; do
  echo "Running $test..."
  ./$test
  echo "$test complete"
  sleep 5
done
```

---

## Analysis Tools

### Packet Capture

```bash
# Capture UDP frames
tcpdump -i any -w vswitch_traffic.pcap port 8080

# Analyze with Wireshark
wireshark vswitch_traffic.pcap
```

### Performance Profiling

```bash
# Profile with gprof
cmake -B build-profile -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-pg"
cmake --build build-profile

# Run and profile
./build-profile/vswitch 8080
gprof build-profile/vswitch gmon.out
```

### Memory Profiling

```bash
# Use Massif from Valgrind
valgrind --tool=massif --massif-out-file=massif.out ./build-debug/vswitch 8080

# Analyze
ms_print massif.out
```

---

## Troubleshooting

### High CPU Usage

**Symptoms**: CPU usage > 90%

**Solutions**:
- Reduce packet rate
- Use Release build
- Check for inefficient loops

### Memory Growth

**Symptoms**: Memory usage increasing over time

**Solutions**:
- Run Valgrind to find leaks
- Check thread cleanup
- Verify RAII usage

### Frame Drops

**Symptoms**: Lost packets during high load

**Solutions**:
- Increase UDP buffer size
- Check socket buffer overflow
- Reduce concurrent connections

### Slow Performance

**Symptoms**: High latency or low throughput

**Solutions**:
- Check kernel buffer sizes
- Use Release builds
- Profile hot code paths

---

## Success Criteria

All stress tests should demonstrate:

1. ✅ **Stability**: No crashes or hangs
2. ✅ **Performance**: Meets benchmark targets
3. ✅ **Reliability**: Low packet loss
4. ✅ **Memory Safety**: No memory leaks
5. ✅ **Thread Safety**: No race conditions
6. ✅ **Scalability**: Handles load increase

---

## Continuous Integration

Add stress tests to CI:

```yaml
# .github/workflows/stress-tests.yml
name: Stress Tests
on:
  schedule:
    - cron: '0 0 * * 0'  # Weekly

jobs:
  stress-test:
    runs-on: self-hosted
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=Release
          cmake --build build
      - name: Run Scale Test
        run: ./tests/stress_scale_test.sh
      - name: Memory Leak Test
        run: |
          valgrind --error-exitcode=1 --leak-check=full \
            ./build/vswitch 8080 &
          sleep 60
          killall vswitch
```

---

## Next Steps

1. Run basic stress tests to establish baselines
2. Identify bottlenecks and optimize
3. Add stress tests to CI pipeline
4. Monitor production for similar patterns
5. Document performance characteristics

