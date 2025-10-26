/**
 * @file tap_device.cpp
 * @brief Implementation of TAP device management
 */

#include "project/tap_device.hpp"

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

// Platform-specific includes
#ifdef __linux__
#include <linux/if.h>
#include <linux/if_tun.h>
#elif __APPLE__
#include <net/if.h>
#include <sys/kern_control.h>
#include <sys/socket.h>
#include <sys/sys_domain.h>
#include <sys/kern_event.h>

// Define UTUN_OPT_IFNAME if not available (varies by macOS version)
#ifndef UTUN_OPT_IFNAME
#define UTUN_OPT_IFNAME 2
#endif
#endif

namespace project
{
  const char* to_string(TapError error) noexcept
  {
    switch (error)
    {
      case TapError::DeviceOpenFailed:
        return "Failed to open /dev/net/tun";
      case TapError::IoctlFailed:
        return "ioctl(TUNSETIFF) failed";
      case TapError::ReadFailed:
        return "Failed to read from TAP device";
      case TapError::WriteFailed:
        return "Failed to write to TAP device";
      case TapError::InvalidDevice:
        return "Invalid TAP device";
      case TapError::PartialWrite:
        return "Partial write to TAP device";
      default:
        return "Unknown TAP error";
    }
  }

  TapDevice::TapDevice(FileDescriptor fd, std::string device_name)
      : fd_(std::move(fd)), device_name_(std::move(device_name))
  {
  }

  expected<TapDevice, TapError> TapDevice::create(std::string_view device_name)
  {
#ifdef __linux__
    // Linux implementation using /dev/net/tun
    int fd = ::open("/dev/net/tun", O_RDWR);
    if (fd < 0)
    {
      return unexpected(TapError::DeviceOpenFailed);
    }

    // Configure the device as TAP (Layer 2, Ethernet frames)
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;  // TAP device, no packet information

    // Set device name if provided
    if (!device_name.empty())
    {
      if (device_name.size() >= IFNAMSIZ)
      {
        ::close(fd);
        return unexpected(TapError::IoctlFailed);
      }
      std::strncpy(ifr.ifr_name, device_name.data(), IFNAMSIZ - 1);
    }

    // Create the TAP device
    if (::ioctl(fd, TUNSETIFF, &ifr) < 0)
    {
      ::close(fd);
      return unexpected(TapError::IoctlFailed);
    }

    // Get the actual device name (kernel may have assigned one)
    std::string actual_name(ifr.ifr_name);

    return TapDevice(FileDescriptor(fd), std::move(actual_name));

#elif __APPLE__
    // macOS implementation using utun devices
    // Note: macOS requires third-party drivers for TAP devices
    // We'll use utun (TUN) as a fallback, or require tuntap installation
    
    // Try to open a utun device (TUN, not TAP, but available natively)
    int fd = ::socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
    if (fd < 0)
    {
      return unexpected(TapError::DeviceOpenFailed);
    }

    // For macOS, we need tuntaposx installed for true TAP support
    // This is a simplified implementation that will fail gracefully
    // In production, you would install: brew install --cask tuntap
    
    struct ctl_info ctl_info;
    std::memset(&ctl_info, 0, sizeof(ctl_info));
    std::strncpy(ctl_info.ctl_name, "com.apple.net.utun_control", sizeof(ctl_info.ctl_name));
    
    if (::ioctl(fd, CTLIOCGINFO, &ctl_info) < 0)
    {
      ::close(fd);
      return unexpected(TapError::IoctlFailed);
    }

    struct sockaddr_ctl sc;
    std::memset(&sc, 0, sizeof(sc));
    sc.sc_id = ctl_info.ctl_id;
    sc.sc_len = sizeof(sc);
    sc.sc_family = AF_SYSTEM;
    sc.ss_sysaddr = AF_SYS_CONTROL;
    sc.sc_unit = 0;  // 0 means kernel will assign unit number

    if (::connect(fd, reinterpret_cast<struct sockaddr*>(&sc), sizeof(sc)) < 0)
    {
      ::close(fd);
      return unexpected(TapError::IoctlFailed);
    }

    // Get the utun device name
    char utun_name[20];
    socklen_t utun_name_len = sizeof(utun_name);
    if (::getsockopt(fd, SYSPROTO_CONTROL, UTUN_OPT_IFNAME, utun_name, &utun_name_len) >= 0)
    {
      std::string actual_name(utun_name);
      return TapDevice(FileDescriptor(fd), std::move(actual_name));
    }
    else
    {
      // Fallback: construct name from unit number
      std::string actual_name = device_name.empty() ? "utun0" : std::string(device_name);
      return TapDevice(FileDescriptor(fd), std::move(actual_name));
    }

#else
    // Unsupported platform
    (void)device_name;  // Suppress unused parameter warning
    return unexpected(TapError::DeviceOpenFailed);
#endif
  }

  expected<std::vector<uint8_t>, TapError> TapDevice::read_frame()
  {
    if (!is_valid())
    {
      return unexpected(TapError::InvalidDevice);
    }

    std::array<uint8_t, ETHER_MAX_LEN> buffer;
    ssize_t n = ::read(fd_.get(), buffer.data(), buffer.size());

    if (n < 0)
    {
      return unexpected(TapError::ReadFailed);
    }

    std::vector<uint8_t> frame(buffer.begin(), buffer.begin() + n);
    return frame;
  }

  expected<size_t, TapError> TapDevice::write_frame(const std::vector<uint8_t>& frame)
  {
    return write_frame(frame.data(), frame.size());
  }

  expected<size_t, TapError> TapDevice::write_frame(const uint8_t* data, size_t size)
  {
    if (!is_valid())
    {
      return unexpected(TapError::InvalidDevice);
    }

    ssize_t n = ::write(fd_.get(), data, size);

    if (n < 0)
    {
      return unexpected(TapError::WriteFailed);
    }

    if (static_cast<size_t>(n) != size)
    {
      return unexpected(TapError::PartialWrite);
    }

    return static_cast<size_t>(n);
  }

}  // namespace project

