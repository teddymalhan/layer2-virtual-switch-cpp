#!/bin/bash
# Stress test for high throughput

set -e

echo "=== VSwitch Throughput Test ==="
echo "Testing high packet rate processing"
echo ""

# Kill any existing processes
pkill vswitch || true
pkill vport || true
sleep 1

# Start VSwitch
echo "Starting VSwitch on port 8080..."
./build/vswitch 8080 &
VSWITCH_PID=$!
sleep 2

# Start two VPorts
echo "Starting VPort 0 on tap0..."
sudo ./build/vport 127.0.0.1 8080 tap0 &
VPORT1_PID=$!
sleep 1

echo "Starting VPort 1 on tap1..."
sudo ./build/vport 127.0.0.1 8080 tap1 &
VPORT2_PID=$!
sleep 1

# Configure TAP devices
sudo ip addr add 10.1.1.101/24 dev tap0
sudo ip addr add 10.1.1.102/24 dev tap1
sudo ip link set tap0 up
sudo ip link set tap1 up

echo ""
echo "System ready for throughput test."
echo ""

# Packet rate test
RATE=100
DURATION=60

echo "Running throughput test:"
echo "  Rate: $RATE packets/second"
echo "  Duration: $DURATION seconds"
echo ""

# Use ping to generate traffic
echo "Starting traffic generation..."
ping -i 0.$RATE -c $((RATE * DURATION)) 10.1.1.102 &
PING_PID=$!

# Monitor progress
sleep $DURATION

wait $PING_PID

echo ""
echo "Throughput test complete!"
echo ""

# Cleanup
echo "Stopping components..."
kill $VSWITCH_PID $VPORT1_PID $VPORT2_PID 2>/dev/null

echo "Done."

