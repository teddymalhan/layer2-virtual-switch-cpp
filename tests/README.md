# Testing Directory

This directory contains all testing-related files and documentation for the VPN project.

## Files

| File | Description |
|------|-------------|
| **QUICK_START.md** | 5-minute guide to get testing |
| **TESTING.md** | Comprehensive testing documentation |
| **README.md** | This file - overview of testing directory |
| **test_in_docker.sh** | Automated Docker test script (recommended) |
| **docker_test.sh** | Alternative Docker test script |
| **test_docker.sh** | Legacy Docker test script |
| **Dockerfile** | Main Docker image definition |
| **Dockerfile.test** | Docker image for testing |
| **Dockerfile.alpine** | Alpine-based Docker image |
| **ALPINE_TEST.md** | Alpine Linux testing notes |

## Quick Links

### Get Started

1. **Just want to test it?** → [QUICK_START.md](QUICK_START.md)
2. **Need detailed testing info?** → [TESTING.md](TESTING.md)
3. **Want to run the automated test?** → Run `./test_in_docker.sh`

### Using the Test Script

```bash
# Make executable
chmod +x test_in_docker.sh

# Run it
./test_in_docker.sh
```

This will:
- Start VSwitch in a Docker container
- Start two VPort clients
- Configure TAP devices
- Test connectivity
- Show logs
- Clean up automatically

### Manual Testing

For detailed manual testing instructions, see [TESTING.md](TESTING.md).

## Docker Images

### Build Main Image

```bash
docker build -t vpn-alpine -f Dockerfile .
```

### Build Test Image

```bash
docker build -t vpn-test -f Dockerfile.test .
```

## Test Results

- **Unit Tests**: 113 tests (all passing)
- **Integration Tests**: 9 test suites (all passing)
- **Docker Tests**: ✅ Working
- **Manual Tests**: ✅ Working on macOS

## Directory Structure

```
tests/
├── README.md              # This file
├── QUICK_START.md         # Quick start guide
├── TESTING.md             # Comprehensive testing guide
├── test_in_docker.sh      # Main automated test script
├── docker_test.sh         # Alternative test script
├── test_docker.sh         # Legacy test script
├── Dockerfile             # Main Docker image
├── Dockerfile.test        # Test Docker image
├── Dockerfile.alpine      # Alpine Docker image
└── ALPINE_TEST.md         # Alpine testing notes
```

## Running Tests

### Automated Testing

```bash
./test_in_docker.sh
```

### Unit Testing (Native)

```bash
cd ../build
cmake ..
cmake --build .
ctest -C Debug -VV
```

### Manual Testing

See [TESTING.md](TESTING.md) for detailed instructions.

## Troubleshooting

Common issues and solutions:

### "Permission denied"
```bash
chmod +x test_in_docker.sh
```

### "Docker image not found"
```bash
docker build -t vpn-alpine -f Dockerfile ..
```

### "Cannot create TAP device"
- Ensure running with `--cap-add=NET_ADMIN`
- Add `--device=/dev/net/tun` to docker run command

### For more help
- See [TESTING.md](TESTING.md) troubleshooting section
- Check main project [README.md](../README.md)

