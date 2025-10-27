[![CI](https://github.com/teddymalhan/modern-cpp-template/workflows/CI/badge.svg)](https://github.com/teddymalhan/modern-cpp-template/actions)
[![Quick Check](https://github.com/teddymalhan/modern-cpp-template/workflows/Quick%20Check/badge.svg)](https://github.com/teddymalhan/modern-cpp-template/actions)
[![codecov](https://codecov.io/gh/teddymalhan/modern-cpp-template/branch/main/graph/badge.svg)](https://codecov.io/gh/teddymalhan/modern-cpp-template)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/filipdutescu/modern-cpp-template)](https://github.com/filipdutescu/modern-cpp-template/releases)

# Modern C++ Virtual Switch Network (VPN)

A modern C++17 implementation of a Layer 2 Virtual Private Network (VPN) / Virtual Switch system.

This project provides a learning switch that connects virtual network interfaces (TAP devices) 
across networks via UDP, enabling Layer 2 Ethernet frame forwarding over IP networks.

## What's Included

- **VSwitch**: Central switching fabric with MAC address learning
- **VPort**: Virtual port connecting TAP devices to the VSwitch
- **Modern C++17**: RAII, std::expected, joining_thread, move semantics
- **Cross-platform**: Works on Linux and macOS
- **Comprehensive tests**: 113 unit and integration tests
- **Executables**: `vport` and `vswitch`

## Quick Start

### Option 1: Docker (Easiest)

```bash
# Build and run automated test
docker build -t vpn-alpine -f tests/Dockerfile .
chmod +x tests/test_in_docker.sh
./tests/test_in_docker.sh
```

### Option 2: Native Build

```bash
# Build
cd /path/to/modern-cpp-template
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Start VSwitch
./build/vswitch 8080

# Start VPort (in another terminal, requires sudo)
sudo ./build/vport 127.0.0.1 8080
```

### Documentation

- **Testing Guide**: [tests/TESTING.md](tests/TESTING.md)
- **Quick Start**: [tests/QUICK_START.md](tests/QUICK_START.md)
- **Usage Guide**: [docs/vpn-usage.md](docs/vpn-usage.md)

## Architecture

The system consists of:
- **VSwitch**: Central switch that learns MAC addresses and forwards frames
- **VPort**: Virtual port bridging TAP device to VSwitch via UDP
- **TAP Device**: Virtual network interface at Layer 2 (Ethernet)

## Features

* Modern **CMake** configuration and project, which, to the best of my
knowledge, uses the best practices,

* **Layer 2 VPN**: Full Ethernet frame forwarding over UDP

* **Learning Switch**: Automatic MAC address learning and forwarding

* **Cross-Platform**: Works on Linux and macOS with proper TAP support

* An example of a **Clang-Format** config, inspired from the base *Google* model,
with minor tweaks. This is aimed only as a starting point, as coding style
is a subjective matter, everyone is free to either delete it (for the *LLVM*
default) or supply their own alternative,

* **Static analyzers** integration, with *Clang-Tidy* and *Cppcheck*, the former
being the default option,

* **Doxygen** support, through the `ENABLE_DOXYGEN` option, which you can enable
if you wish to use it,

* **Unit testing** support, through *GoogleTest* (with an option to enable
*GoogleMock*) or *Catch2*,

* **Code coverage**, enabled by using the `ENABLE_CODE_COVERAGE` option, through
*Codecov* CI integration,

* **Package manager support**, with *Conan* and *Vcpkg*, through their respective
options

* **CI workflows for Windows, Linux and MacOS** using *GitHub Actions*, making
use of the caching features, to ensure minimum run time,

* **.md templates** for: *README*, *Contributing Guideliness*,
*Issues* and *Pull Requests*,

* **Permissive license** to allow you to integrate it as easily as possible. The
template is licensed under the [Unlicense](https://unlicense.org/),

* Options to build as a header-only library or executable, not just a static or
shared library.

* **Ccache** integration, for speeding up rebuild times

## Getting Started

These instructions will get you a copy of the project up and running on your local
machine for development and testing purposes.

### Prerequisites

This project is meant to be only a template, thus versions of the software used
can be change to better suit the needs of the developer(s). If you wish to use the
template *as-is*, meaning using the versions recommended here, then you will need:

* **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)

* **C++ Compiler** - needs to support at least the **C++17** standard, i.e. *MSVC*,
*GCC*, *Clang*

> ***Note:*** *You also need to be able to provide ***CMake*** a supported
[generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).*

### Installing

It is fairly easy to install the project, all you need to do is clone if from
[GitHub](https://github.com/filipdutescu/modern-cpp-template) or
[generate a new repository from it](https://github.com/filipdutescu/modern-cpp-template/generate)
(also on **GitHub**).

If you wish to clone the repository, rather than generate from it, you simply need
to run:

```bash
git clone https://github.com/filipdutescu/modern-cpp-template/
```

After finishing getting a copy of the project, with any of the methods above, create
a new folder in the `include/` folder, with the name of your project.  Edit
`cmake/SourcesAndHeaders.cmake` to add your files.

You will also need to rename the `cmake/ProjectConfig.cmake.in` file to start with
the ***exact name of your project***. Such as `cmake/MyNewProjectConfig.cmake.in`.
You should also make the same changes in the GitHub workflows provided, notably
[`.github/workflows/ubuntu.yml`](.github/workflows/ubuntu.yml), in which you should
replace the CMake option `-DProject_ENABLE_CODE_COVERAGE=1` to
`-DMyNewProject_ENABLE_CODE_COVERAGE=1`.

Finally, change `"Project"` from `CMakeLists.txt`, from

```cmake
project(
  "Project"
  VERSION 0.1.0
  LANGUAGES CXX
)
```

to the ***exact name of your project***, i.e. using the previous name it will become:

```cmake
project(
  MyNewProject
  VERSION 0.1.0
  LANGUAGES CXX
)
```

To install an already built project, you need to run the `install` target with CMake.
For example:

```bash
cmake --build build --target install --config Release

# a more general syntax for that command is:
cmake --build <build_directory> --target install --config <desired_config>
```

## Building the project

To build the project, all you need to do, ***after correctly
[installing the project](README.md#Installing)***, is run a similar **CMake** routine
to the the one below:

```bash
mkdir build/ && cd build/
cmake .. -DCMAKE_INSTALL_PREFIX=/absolute/path/to/custom/install/directory
cmake --build . --target install
```

> ***Note:*** *The custom ``CMAKE_INSTALL_PREFIX`` can be omitted if you wish to
install in [the default install location](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html).*

More options that you can set for the project can be found in the
[`cmake/StandardSettings.cmake` file](cmake/StandardSettings.cmake). For certain
options additional configuration may be needed in their respective `*.cmake` files
(i.e. Conan needs the `CONAN_REQUIRES` and might need the `CONAN_OPTIONS` to be setup
for it work correctly; the two are set in the [`cmake/Conan.cmake` file](cmake/Conan.cmake)).

## Generating the documentation

In order to generate documentation for the project, you need to configure the build
to use Doxygen. This is easily done, by modifying the workflow shown above as follows:

```bash
mkdir build/ && cd build/
cmake .. -D<project_name>_ENABLE_DOXYGEN=1 -DCMAKE_INSTALL_PREFIX=/absolute/path/to/custom/install/directory
cmake --build . --target doxygen-docs
```

> ***Note:*** *This will generate a `docs/` directory in the **project's root directory**.*

## Running the tests

By default, the template uses [Google Test](https://github.com/google/googletest/)
for unit testing. Unit testing can be disabled in the options, by setting the
`ENABLE_UNIT_TESTING` (from
[cmake/StandardSettings.cmake](cmake/StandardSettings.cmake)) to be false. To run
the tests, simply use CTest, from the build directory, passing the desire
configuration for which to run tests for. An example of this procedure is:

```bash
cd build          # if not in the build directory already
ctest -C Release  # or `ctest -C Debug` or any other configuration you wish to test

# you can also run tests with the `-VV` flag for a more verbose output (i.e.
#GoogleTest output as well)
```

### End to end tests

If applicable, should be presented here.

### Coding style tests

If applicable, should be presented here.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our how you can
become a contributor and the process for submitting pull requests to us.

## Versioning

This project makes use of [SemVer](http://semver.org/) for versioning. A list of
existing versions can be found in the
[project's releases](https://github.com/filipdutescu/modern-cpp-template/releases).

## Authors

* **Filip-Ioan Dutescu** - [@filipdutescu](https://github.com/filipdutescu)

## License

This project is licensed under the [Unlicense](https://unlicense.org/) - see the
[LICENSE](LICENSE) file for details
