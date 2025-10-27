[![CI](https://github.com/teddymalhan/modern-cpp-template/workflows/CI/badge.svg)](https://github.com/teddymalhan/modern-cpp-template/actions)
[![Quick Check](https://github.com/teddymalhan/modern-cpp-template/workflows/Quick%20Check/badge.svg)](https://github.com/teddymalhan/modern-cpp-template/actions)
[![codecov](https://codecov.io/gh/teddymalhan/modern-cpp-template/branch/main/graph/badge.svg)](https://codecov.io/gh/teddymalhan/modern-cpp-template)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/the-standard)
[![License: Unlicense](https://img.shields.io/badge/License-Unlicense-lightgrey.svg)](https://unlicense.org/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS-green.svg)](https://github.com/teddymalhan/modern-cpp-template)

# 🔷 Modern C++ Virtual Switch Network (VPN)

A **production-ready** C++17 implementation of a Layer 2 Virtual Private Network (VPN) / Virtual Switch system featuring MAC address learning, Ethernet frame forwarding over UDP, and a modern CMake-based build system.

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Quick Start](#-quick-start)
- [Architecture](#-architecture)
- [Project Structure](#-project-structure)
- [Building](#-building)
- [Testing](#-testing)
- [Usage](#-usage)
- [Documentation](#-documentation)
- [Technology Stack](#-technology-stack)
- [Contributing](#-contributing)

---

## 🎯 Overview

This project implements a **learning switch** that connects virtual network interfaces (TAP devices) across networks via UDP, enabling Layer 2 Ethernet frame forwarding over IP networks. It demonstrates modern C++ best practices including RAII, move semantics, type-safe error handling, and zero-copy forwarding.

### What It Does

- **VSwitch**: Central switching fabric with MAC address learning
- **VPort**: Virtual port connecting TAP devices to the VSwitch
- **Automatic Learning**: Learns MAC addresses dynamically
- **Frame Forwarding**: Routes Ethernet frames efficiently
- **Cross-Platform**: Works seamlessly on Linux and macOS

---

## ✨ Features

### Core Functionality
- ✅ **Layer 2 VPN** - Full Ethernet frame forwarding over UDP
- ✅ **Learning Switch** - Automatic MAC address learning and forwarding
- ✅ **Zero Memory Leaks** - RAII ensures automatic cleanup
- ✅ **Thread-Safe** - Uses `std::shared_mutex` for concurrent access
- ✅ **Type-Safe Errors** - Uses `std::expected` for error handling

### Modern C++17
- ✅ **RAII** - Automatic resource management
- ✅ **Move Semantics** - Efficient resource transfer
- ✅ **Smart Pointers** - Memory safety without manual management
- ✅ **std::expected** - Type-safe error handling
- ✅ **joining_thread** - RAII thread wrapper

### Development Experience
- ✅ **Comprehensive Tests** - 113 unit and integration tests
- ✅ **Modern CMake** - Industry best practices
- ✅ **CI/CD** - GitHub Actions for multi-platform builds
- ✅ **Code Coverage** - Integrated with Codecov
- ✅ **Static Analysis** - Clang-Tidy and Cppcheck
- ✅ **Documentation** - Doxygen support

### Cross-Platform
- ✅ **Linux** - Native TAP device support
- ✅ **macOS** - utun interface support
- ✅ **Docker** - Containerized testing and deployment

---

## 🚀 Quick Start

### Option 1: Docker (Recommended)

```bash
# Build and run automated test
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

### Option 2: Native Build

```bash
# Build
cd /path/to/modern-cpp-template
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Start VSwitch
./build/vswitch 8080

# Start VPort (requires sudo)
sudo ./build/vport 127.0.0.1 8080
```

### Prerequisites

- **CMake** v3.15+ ([Download](https://cmake.org/))
- **C++ Compiler** supporting C++17 (GCC 7+, Clang 5+, MSVC 2017+)
- **Root privileges** for TAP device creation
- **Python 3** (for VSwitch server)

---

## 🏗️ Architecture

```
┌──────────────────────────────────────────┐
│           VSwitch (Server)               │
│  • Learns MAC addresses                  │
│  • Forwards frames to known hosts        │
│  • Handles broadcast frames              │
│  • UDP-based central switching fabric    │
└────────────┬─────────────────────────────┘
             │ UDP
             │
   ┌─────────┴─────────┬──────────────┐
   │                   │              │
VPort 1           VPort 2        VPort N
(Client)          (Client)       (Client)
   │                   │              │
   └─── TAP ───────────┴────── TAP ───┘
         │                │              │
   Linux Kernel    Linux Kernel   Linux Kernel
```

### Components

#### VSwitch
- Listens on UDP port for incoming frames
- Learns MAC addresses from source MACs
- Forwards unicast frames to known destinations
- Broadcasts frames to all known endpoints
- Discards unknown unicast frames

#### VPort
- Creates TAP device (virtual network interface)
- Connects to VSwitch via UDP
- Two forwarder threads (bidirectional)
- Handles frame encapsulation/decap of Ethernet frames

#### MacTable
- Thread-safe MAC address learning table
- Uses `std::shared_mutex` for efficient concurrent reads
- O(1) MAC lookup with `std::unordered_map`
- Supports broadcast exclusion

---

## 📁 Project Structure

```
modern-cpp-template/
├── CMakeLists.txt                 # Main build configuration
├── Makefile                       # Convenience build commands
├── README.md                      # This file
├── PROJECT_OVERVIEW.md           # Complete overview
│
├── cmake/                         # CMake modules
│   ├── CompilerWarnings.cmake    # Warning flags
│   ├── Doxygen.cmake             # Documentation config
│   ├── SourcesAndHeaders.cmake   # Source lists
│   ├── StandardSettings.cmake    # Build options
│   ├── StaticAnalyzers.cmake     # Analysis config
│   └── Utils.cmake               # Helper functions
│
├── include/project/               # Public headers
│   ├── expected.hpp              # std::expected implementation
│   ├── joining_thread.hpp        # RAII thread wrapper
│   ├── sys_utils.hpp            # System utilities
│   ├── tap_device.hpp            # TAP device management
│   ├── ethernet_frame.hpp        # Ethernet frame parsing
│   ├── udp_socket.hpp            # UDP socket wrapper
│   ├── vport.hpp                 # Virtual Port
│   ├── mac_table.hpp             # MAC address table
│   ├── vswitch.hpp               # Virtual Switch
│   └── tmp.hpp                   # Template code
│
├── src/                           # Implementation
│   ├── vport_main.cpp            # VPort executable
│   ├── vswitch_main.cpp          # VSwitch executable
│   └── [other implementation files]
│
├── test/                          # Test configuration
│   ├── CMakeLists.txt
│   └── src/                      # Test files
│       ├── integration_test.cpp
│       ├── tap_device_test.cpp
│       ├── udp_socket_test.cpp
│       └── [113 tests total]
│
├── tests/                         # Testing scripts & docs
│   ├── README.md                 # Testing overview
│   ├── QUICK_START.md            # 5-minute testing guide
│   ├── TESTING.md                # Comprehensive guide
│   ├── test_in_docker.sh         # Automated Docker test
│   └── Dockerfile                # Docker test image
│
└── docs/                          # Documentation
    ├── vpn-usage.md               # Usage guide
    ├── architecture-overview.md   # Architecture diagrams
    ├── class-diagram.md           # Class relationships
    └── sequence-diagram.md        # Interaction flows
```

---

## 🔨 Building

### Basic Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Install (optional)
cmake --build build --target install --config Release
```

### Build Options

```cmake
# With all features
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DProject_ENABLE_UNIT_TESTING=1 \
  -DProject_ENABLE_CLANG_TIDY=1 \
  -DProject_ENABLE_CODE_COVERAGE=1 \
  -DProject_ENABLE_DOXYGEN=1 \
  -DProject_ENABLE_LTO=1

cmake --build build
```

### Build Targets

| Target | Description |
|--------|-------------|
| `vswitch` | Virtual Switch executable |
| `vport` | Virtual Port executable |
| `all` | Build all targets (default) |
| `install` | Install to system |
| `clang-format` | Format code with clang-format |
| `doxygen-docs` | Generate Doxygen documentation |
| `test` | Run tests |

### Common Commands

```bash
# Build everything
make

# Run tests
make test

# Format code
make format

# Generate docs
make docs

# Clean
make clean
```

---

## 🧪 Testing

### Test Statistics

- **Total Tests**: 113
- **Test Suites**: 9
- **Coverage**: Integrated with Codecov
- **Status**: ✅ All passing

### Run Tests

```bash
# All tests
cd build && ctest -C Debug -VV

# Specific test
./build/test/tmp_test_Tests

# With Makefile
make test
```

### Test Categories

1. **Unit Tests** - Component-level tests
2. **Integration Tests** - Multi-component interaction
3. **Docker Tests** - End-to-end testing
4. **Manual Tests** - Verified on macOS
5. **Stress Tests** - Performance and stability testing (see [tests/STRESS_TESTING.md](tests/STRESS_TESTING.md))

### Automated Docker Testing

```bash
# Run automated test suite
chmod +x tests/test_in_docker.sh
./tests/test_in_docker.sh
```

### Stress Testing

```bash
# Run comprehensive stress tests
./tests/stress_comprehensive.sh

# Or run individual tests
./tests/stress_scale_test.sh      # Multiple VPort connections
./tests/stress_throughput_test.sh # High packet rate
./tests/stress_duration_test.sh   # Long-running stability
```

### Testing Documentation

- **Quick Start**: [tests/QUICK_START.md](tests/QUICK_START.md) - Get started in 5 minutes
- **Comprehensive Guide**: [tests/TESTING.md](tests/TESTING.md) - Detailed testing documentation
- **Stress Testing**: [tests/STRESS_TESTING.md](tests/STRESS_TESTING.md) - Performance and stability testing
- **Industry Standard**: [tests/INDUSTRY_STANDARD_STRESS_TESTING.md](tests/INDUSTRY_STANDARD_STRESS_TESTING.md) - RFC 2544 compliant testing

---

## 💻 Usage

### Basic Usage

```bash
# Terminal 1: Start VSwitch
./build/vswitch 8080

# Terminal 2: Start VPort 1
sudo ./build/vport 127.0.0.1 8080

# Terminal 3: Start VPort 2
sudo ./build/vport 127.0.0.1 8080
```

### Configure TAP Devices

```bash
# Get device names from VPort output
# Linux
sudo ip addr add 10.1.1.101/24 dev tap0
sudo ip link set tap0 up

# macOS
sudo ifconfig utun6 inet 10.1.1.101 netmask 255.255.255.0
sudo ifconfig utun6 up
```

### Test Connectivity

```bash
# Ping between VPorts
ping 10.1.1.102

# You should see:
# - VSwitch learning MAC addresses
# - Frame forwarding logs
# - Successful connectivity
```

### Complete Usage Guide

For detailed usage instructions, see [docs/vpn-usage.md](docs/vpn-usage.md).

---

## 📚 Documentation

### Available Documentation

| Document | Description |
|----------|-------------|
| [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) | Complete project overview |
| [docs/vpn-usage.md](docs/vpn-usage.md) | Usage guide and examples |
| [docs/architecture-overview.md](docs/architecture-overview.md) | System architecture |
| [tests/QUICK_START.md](tests/QUICK_START.md) | 5-minute testing guide |
| [tests/TESTING.md](tests/TESTING.md) | Comprehensive testing guide |
| [CONTRIBUTING.md](CONTRIBUTING.md) | Contribution guidelines |

### Generate Documentation

```bash
# Enable Doxygen
cmake -B build -DProject_ENABLE_DOXYGEN=1

# Build docs
cmake --build build --target doxygen-docs

# Or use Makefile
make docs
```

---

## 🛠️ Technology Stack

### Languages & Standards
- **C++17** - Modern C++ features
- **CMake 3.15+** - Build system
- **Python 3** - VSwitch server

### Libraries & Tools
- **GoogleTest** - Unit testing framework
- **Doxygen** - Documentation generation
- **Clang-Tidy** - Static analysis
- **Cppcheck** - Additional static analysis
- **Codecov** - Code coverage tracking

### CI/CD
- **GitHub Actions** - Multi-platform builds
- **Docker** - Containerized testing
- **Automated Testing** - On every commit

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `ENABLE_UNIT_TESTING` | Enable GoogleTest | ON |
| `ENABLE_CLANG_TIDY` | Enable Clang-Tidy | OFF |
| `ENABLE_CPPCHECK` | Enable Cppcheck | OFF |
| `ENABLE_CODE_COVERAGE` | Enable coverage | OFF |
| `ENABLE_DOXYGEN` | Enable Doxygen | OFF |
| `ENABLE_CONAN` | Use Conan package manager | OFF |
| `ENABLE_VCPKG` | Use Vcpkg package manager | OFF |
| `ENABLE_LTO` | Link-time optimization | OFF |
| `ENABLE_ASAN` | Address Sanitizer | OFF |
| `WARNINGS_AS_ERRORS` | Treat warnings as errors | OFF |

---

## 🤝 Contributing

We welcome contributions! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Workflow

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Code Standards

- Follow the existing code style (Google-based with Allman braces)
- Write tests for new features
- Update documentation as needed
- Run `make format` before committing
- All tests must pass

### Pull Request Template

We use a PR template. See [PULL_REQUEST_TEMPLATE.md](PULL_REQUEST_TEMPLATE.md).

---

## 📊 Project Status

| Component | Status |
|-----------|--------|
| VSwitch | ✅ Complete |
| VPort | ✅ Complete |
| Unit Tests | ✅ 113 tests passing |
| Integration Tests | ✅ All passing |
| Docker Tests | ✅ Working |
| Documentation | ✅ Complete |
| CI/CD | ✅ Multi-platform |

---

## 📄 License

This project is licensed under the [Unlicense](https://unlicense.org/) - see the [LICENSE](LICENSE) file for details.

---

## 👥 Authors

- **Teddy Malhan** - [@teddymalhan](https://github.com/teddymalhan)

Based on [modern-cpp-template](https://github.com/filipdutescu/modern-cpp-template) by [Filip Ioan Dutescu](https://github.com/filipdutescu).

---

## 🔗 Links

- **Issues**: [Report a bug](https://github.com/teddymalhan/modern-cpp-template/issues)
- **Discussions**: [Ask a question](https://github.com/teddymalhan/modern-cpp-template/discussions)
- **Wiki**: [View documentation](https://github.com/teddymalhan/modern-cpp-template/wiki)

---

## 📈 Acknowledgments

- Modern CMake best practices from [CLIUtils/modern-cmake](https://github.com/CLIUtils/modern-cmake)
- GoogleTest for testing infrastructure
- All contributors and users of this project
