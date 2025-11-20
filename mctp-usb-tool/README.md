# MCTP USB Tool

## Overview
This is a simple BitBake recipe that builds a C utility for USB device identification and port path discovery. The tool takes a USB Vendor ID and Device ID as parameters, locates the device, and displays detailed information including the USB port path hierarchy.

## Directory Structure
```
mctp-usb-tool/
├── mctp-usb-tool.bb          # BitBake recipe file (Make-based)
├── mctp-usb-tool-meson.bb    # BitBake recipe file (Meson-based)
├── files/                    # Source files directory
│   ├── src/
│   │   └── mctp-usb-tool.c   # Main C source file
│   ├── tests/
│   │   └── test-mctp-usb-tool.c # Test file
│   ├── Makefile              # Make build configuration
│   ├── meson.build           # Meson build configuration
│   ├── meson_options.txt     # Meson build options
│   ├── mctp-usb-tool.conf.in # Configuration template
│   └── mctp-usb-tool.1       # Manual page
└── README.md                 # This file
```

## Features
- USB device discovery by Vendor ID and Device ID
- USB port path identification and display
- Device information display (class, version, etc.)
- Bus number and device address reporting
- Hub hierarchy analysis
- Simple command-line interface

## Usage
After building and installing the package, you can use the tool as follows:

```bash
# Find USB device with vendor ID 0x1234 and device ID 0x5678
mctp-usb-tool 0x1234 0x5678

# Same using decimal values
mctp-usb-tool 4660 22136

# Example with real device IDs (e.g., Intel device)
mctp-usb-tool 0x8086 0x15d4
```

## Arguments
- `vendor_id`: USB Vendor ID (hexadecimal like 0x1234 or decimal like 1234)
- `device_id`: USB Device ID (hexadecimal like 0x5678 or decimal like 5678)

## Output Information
The tool displays:
- Device information (Vendor ID, Product ID, Device Class, Version)
- USB port path showing the physical connection hierarchy
- Bus number and device address
- Hub depth and root port information

## Building

### Using BitBake (Make-based)
This package can be built using BitBake as part of the Yocto/OpenEmbedded build system:

```bash
bitbake mctp-usb-tool
```

### Using BitBake (Meson-based)
Alternatively, you can use the Meson-based build:

```bash
bitbake mctp-usb-tool-meson
```

### Local Development with Meson
For local development, you can use Meson directly:

```bash
# Setup build directory
meson setup builddir

# Configure options (optional)
meson configure builddir -Dsystemd=enabled -Dtests=true

# Build
meson compile -C builddir

# Run tests
meson test -C builddir

# Install
meson install -C builddir
```

### Local Development with Make
For local development with Make:

```bash
make
make install
```

## Configuration
The tool installs a configuration file at `/etc/mctp-usb-tool/mctp-usb-tool.conf` which can be used to set default parameters.

## License
Apache-2.0

## Dependencies

### Runtime Dependencies
- glibc (C library)
- libusb (USB communication library)

### Build Dependencies
#### Make-based build:
- gcc (C compiler)
- make

#### Meson-based build:
- gcc (C compiler)  
- meson (build system)
- ninja (build backend)
- systemd (optional, for systemd integration)