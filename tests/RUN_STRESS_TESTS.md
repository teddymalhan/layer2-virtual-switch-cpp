# Running Stress Tests Manually in Docker

Quick copy-paste commands for running stress tests with Docker.

## Prerequisites

```bash
# Ensure you're in the project root
cd /path/to/modern-cpp-template

# Build the project first
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## Option 1: Automated Docker Test (Recommended)

```bash
# Make executable and run
chmod +x tests/test_in_docker.sh
./tests/test_in_docker.sh
```

This handles everything automatically!

---

## Option 2: Manual Docker Commands

### Step 1: Build Docker Image

```bash
docker build -t vpn-alpine -f tests/Dockerfile .
```

### Step 2: Start VSwitch

```bash
docker run -d \
  --name vswitch-test \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  vpn-alpine \
  /app/build/vswitch 8080
```

Check logs:
```bash
docker logs vswitch-test
```

### Step 3: Start VPort 1

```bash
docker run -d \
  --name vport1-test \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  --network container:vswitch-test \
  vpn-alpine \
  /app/build/vport 127.0.0.1 8080 tap0
```

### Step 4: Start VPort 2

```bash
docker run -d \
  --name vport2-test \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  --network container:vswitch-test \
  vpn-alpine \
  /app/build/vport 127.0.0.1 8080 tap1
```

### Step 5: Configure TAP Devices

```bash
# Configure VPort 1
docker exec vport1-test ip addr add 10.1.1.101/24 dev tap0
docker exec vport1-test ip link set tap0 up

# Configure VPort 2
docker exec vport2-test ip addr add 10.1.1.102/24 dev tap1
docker exec vport2-test ip link set tap1 up
```

### Step 6: Test Connectivity

```bash
# Ping from VPort 1 to VPort 2
docker exec vport1-test ping -c 10 10.1.1.102

# Ping from VPort 2 to VPort 1
docker exec vport2-test ping -c 10 10.1.1.101
```

### Step 7: Check Logs

```bash
# VSwitch logs
docker logs vswitch-test

# VPort 1 logs
docker logs vport1-test

# VPort 2 logs
docker logs vport2-test

# Follow logs in real-time
docker logs -f vswitch-test
```

### Step 8: Cleanup

```bash
docker stop vswitch-test vport1-test vport2-test
docker rm vswitch-test vport1-test vport2-test
```

---

## Option 3: Stress Test - Multiple VPorts

### Start 5 VPorts for Scale Test

```bash
# Start VSwitch (if not already running)
docker run -d \
  --name vswitch-test \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  vpn-alpine \
  /app/build/vswitch 8080

# Start 5 VPorts
for i in {1..5}; do
  docker run -d \
    --name vport${i}-test \
    --cap-add=NET_ADMIN \
    --device=/dev/net/tun \
    --network container:vswitch-test \
    vpn-alpine \
    /app/build/vport 127.0.0.1 8080 tap$i
  
  # Configure TAP device
  docker exec vport${i}-test ip addr add 10.1.1.$((100 + i))/24 dev tap$i
  docker exec vport${i}-test ip link set tap$i up
  
  echo "VPort $i configured"
done
```

### Test Connectivity Between All Pairs

```bash
# Ping from each VPort to every other VPort
for src in {1..5}; do
  for dst in {1..5}; do
    if [ $src -ne $dst ]; then
      echo "Testing: VPort $src → VPort $dst"
      docker exec vport${src}-test ping -c 3 -W 1 10.1.1.$((100 + dst))
    fi
  done
done
```

### Generate High-Rate Traffic

```bash
# High-rate ping from VPort 1 to VPort 2 (100 packets)
docker exec vport1-test ping -i 0.1 -c 100 10.1.1.102

# Flood ping (maximum rate)
docker exec vport1-test ping -f 10.1.1.102

# Run for 10 minutes
docker exec vport1-test bash -c "for i in {1..600}; do ping -c 1 -W 1 10.1.1.102; sleep 1; done"
```

---

## Option 4: Stress Test - Throughput Test

### Start Two VPorts and Generate High Traffic

```bash
# Start VSwitch
docker run -d \
  --name vswitch-throughput \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  vpn-alpine \
  /app/build/vswitch 8080

# Start VPort 1
docker run -d \
  --name vport-throughput1 \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  --network container:vswitch-throughput \
  vpn-alpine \
  /app/build/vport 127.0.0.1 8080 tap0

# Start VPort 2
docker run -d \
  --name vport-throughput2 \
  --cap-add=NET_ADMIN \
  --device=/dev/net/tun \
  --network container:vswitch-throughput \
  vpn-alpine \
  /app/build/vport 127.0.0.1 8080 tap1

# Configure
docker exec vport-throughput1 ip addr add 10.1.1.101/24 dev tap0
docker exec vport-throughput1 ip link set tap0 up

docker exec vport-throughput2 ip addr add 10.1.1.102/24 dev tap1
docker exec vport-throughput2 ip link set tap1 up

# Generate high-rate traffic (100 packets per second, 1000 packets)
docker exec vport-throughput1 ping -i 0.01 -c 1000 10.1.1.102

# Or continuous flooding
docker exec vport-throughput1 ping -f 10.1.1.102
# Run for 30 seconds, then Ctrl+C
```

---

## Option 5: Stress Test - Duration Test (Long-Running)

### Run for 1 Hour with Periodic Traffic

```bash
# Start system (use commands from Option 3)

# Generate traffic every 10 seconds for 1 hour
docker exec vport1-test bash -c "
  for i in {1..360}; do
    ping -c 1 -W 1 10.1.1.102
    sleep 10
    echo \"Minute \$((i / 6)): Check \$i complete\"
  done
"

# Or run in background
docker exec -d vport1-test bash -c "
  for i in {1..360}; do
    ping -c 1 -W 1 10.1.1.102 > /dev/null 2>&1
    sleep 10
  done
"

# Monitor logs during the test
docker logs -f vswitch-throughput
```

---

## Option 6: Comprehensive Stress Test

### Run All Stress Scenarios

```bash
# Start VSwitch and 4 VPorts
docker run -d --name vswitch-stress --cap-add=NET_ADMIN --device=/dev/net/tun \
  vpn-alpine /app/build/vswitch 8080

for i in {1..4}; do
  docker run -d \
    --name vport-stress${i} \
    --cap-add=NET_ADMIN \
    --device=/dev/net/tun \
    --network container:vswitch-stress \
    vpn-alpine \
    /app/build/vport 127.0.0.1 8080 tap$i
  
  docker exec vport-stress${i} ip addr add 10.1.1.$((100 + i))/24 dev tap$i
  docker exec vport-stress${i} ip link set tap$i up
done

# Test connectivity
for src in {1..4}; do
  for dst in {1..4}; do
    if [ $src -ne $dst ]; then
      docker exec vport-stress${src} ping -c 1 -W 1 10.1.1.$((100 + dst)) > /dev/null 2>&1 && \
        echo "✓ VPort $src → VPort $dst: OK"
    fi
  done
done

# Generate continuous traffic for 5 minutes
for i in {1..300}; do
  echo "Iteration $i"
  for src in {1..4}; do
    for dst in {1..4}; do
      if [ $src -ne $dst ]; then
        docker exec vport-stress${src} ping -c 1 -W 1 10.1.1.$((100 + dst)) > /dev/null 2>&1
      fi
    done
  done
  sleep 1
done

# Cleanup
docker stop vswitch-stress vport-stress{1..4}
docker rm vswitch-stress vport-stress{1..4}
```

---

## Monitoring During Tests

### Watch Logs in Real-Time

```bash
# VSwitch logs
docker logs -f vswitch-test

# All containers
docker logs -f vswitch-test vport1-test vport2-test
```

### Check System Resources

```bash
# Container stats
docker stats vswitch-test vport1-test vport2-test

# Inside container
docker exec vswitch-test top -b -n 1
docker exec vport1-test ifconfig
```

### Monitor Network Traffic

```bash
# Capture packets
docker exec vport1-test tcpdump -i tap0 -c 50

# Monitor TAP devices
docker exec vport1-test ip link show
docker exec vport1-test ip addr show
```

---

## Troubleshooting

### Check if VSwitch is running

```bash
docker ps | grep vswitch
docker logs vswitch-test
```

### Restart a container

```bash
docker restart vport1-test
```

### Clean up everything

```bash
docker stop $(docker ps -a -q --filter "name=vswitch\|vport")
docker rm $(docker ps -a -q --filter "name=vswitch\|vport")
```

### Check TAP devices

```bash
docker exec vport1-test ip link show type tun
docker exec vport1-test ls -l /dev/net/tun
```

### Force cleanup of stuck containers

```bash
docker kill $(docker ps -a -q --filter "name=vswitch\|vport")
docker rm $(docker ps -a -q --filter "name=vswitch\|vport")
```

---

## Quick Reference

```bash
# Start minimal test (2 VPorts)
docker run -d --name vswitch --cap-add=NET_ADMIN --device=/dev/net/tun \
  vpn-alpine /app/build/vswitch 8080

docker run -d --name vport1 --cap-add=NET_ADMIN --device=/dev/net/tun \
  --network container:vswitch vpn-alpine /app/build/vport 127.0.0.1 8080 tap0

docker run -d --name vport2 --cap-add=NET_ADMIN --device=/dev/net/tun \
  --network container:vswitch vpn-alpine /app/build/vport 127.0.0.1 8080 tap1

# Configure
docker exec vport1 ip addr add 10.1.1.101/24 dev tap0 && \
docker exec vport1 ip link set tap0 up && \
docker exec vport2 ip addr add 10.1.1.102/24 dev tap1 && \
docker exec vport2 ip link set tap1 up

# Test
docker exec vport1 ping -c 5 10.1.1.102

# Cleanup
docker stop vswitch vport1 vport2 && docker rm vswitch vport1 vport2
```

---

## Next Steps

- See [STRESS_TESTING.md](STRESS_TESTING.md) for detailed analysis
- Check [TESTING.md](TESTING.md) for debugging help
- Monitor logs for errors during stress tests

