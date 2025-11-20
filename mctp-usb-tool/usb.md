# USB Concepts and MCTP Implementation Guide

## Table of Contents
1. [USB Architecture Overview](#usb-architecture-overview)
2. [USB Device Hierarchy](#usb-device-hierarchy)
3. [USB Configurations](#usb-configurations)
4. [USB Interfaces](#usb-interfaces)
5. [USB Endpoints](#usb-endpoints)
6. [USB Host vs Device](#usb-host-vs-device)
7. [MCTP USB Implementation](#mctp-usb-implementation)
8. [Port Paths and Device Identification](#port-paths-and-device-identification)
9. [Code Examples](#code-examples)
10. [Troubleshooting](#troubleshooting)

---

## USB Architecture Overview

USB (Universal Serial Bus) follows a **tree topology** with a clear **master-slave** relationship:

```
USB Host (Master)
    ├── USB Hub
    │   ├── USB Device 1
    │   ├── USB Device 2
    │   └── USB Device 3
    └── USB Device 4
```

### Key Principles:
- **Host controls all communication** - devices cannot initiate transfers
- **Enumeration process** - host discovers and configures devices
- **Standardized descriptors** - devices describe their capabilities
- **Multiple transfer types** - control, bulk, interrupt, isochronous

---

## USB Device Hierarchy

Every USB device has a **hierarchical structure** of descriptors:

```
USB Device
├── Device Descriptor (basic device info)
└── Configuration 1 (power, interfaces)
    ├── Interface 0 (functional capability)
    │   ├── Interface Descriptor
    │   ├── Endpoint 1 OUT
    │   └── Endpoint 1 IN
    └── Interface 1 (another capability)
        ├── Interface Descriptor
        ├── Endpoint 2 OUT
        └── Endpoint 3 IN
```

### Device Descriptor
Contains basic device information:
- **Vendor ID (VID)**: Manufacturer identifier
- **Product ID (PID)**: Product identifier  
- **Device Class**: General device type
- **Number of Configurations**: Available configurations

**Example**:
```c
struct usb_device_descriptor {
    uint8_t  bLength;            // Descriptor size
    uint8_t  bDescriptorType;    // Device descriptor type
    uint16_t bcdUSB;             // USB specification version
    uint8_t  bDeviceClass;       // Device class code
    uint8_t  bDeviceSubClass;    // Device subclass code
    uint8_t  bDeviceProtocol;    // Device protocol code
    uint8_t  bMaxPacketSize0;    // Max packet size for endpoint 0
    uint16_t idVendor;           // Vendor ID
    uint16_t idProduct;          // Product ID
    uint16_t bcdDevice;          // Device release number
    uint8_t  iManufacturer;      // Manufacturer string index
    uint8_t  iProduct;           // Product string index
    uint8_t  iSerialNumber;      // Serial number string index
    uint8_t  bNumConfigurations; // Number of configurations
};
```

---

## USB Configurations

A **configuration** represents a **power mode** and **set of interfaces** that the device can operate in.

### Configuration Descriptor
- **Power requirements**: Bus-powered vs self-powered
- **Maximum power consumption**: In 2mA units
- **Number of interfaces**: Functional capabilities available
- **Configuration value**: Used to select this configuration

**Example**:
```c
struct usb_config_descriptor {
    uint8_t  bLength;             // Descriptor size
    uint8_t  bDescriptorType;     // Configuration descriptor type
    uint16_t wTotalLength;        // Total length of data returned
    uint8_t  bNumInterfaces;      // Number of interfaces
    uint8_t  bConfigurationValue; // Configuration ID
    uint8_t  iConfiguration;      // Configuration string index
    uint8_t  bmAttributes;        // Power attributes
    uint8_t  bMaxPower;           // Maximum power consumption
};
```

### Multiple Configurations Example:
```
USB Device
├── Configuration 1 (High Power Mode)
│   ├── Interface 0: MCTP
│   └── Interface 1: Mass Storage
└── Configuration 2 (Low Power Mode)
    └── Interface 0: MCTP Only
```

---

## USB Interfaces

An **interface** represents a **functional capability** or **feature** of the device.

### Interface Characteristics:
- **Interface Class**: Defines the type of functionality (HID, CDC, MCTP, etc.)
- **Interface Subclass**: Refines the functionality
- **Interface Protocol**: Specific protocol within the class
- **Endpoints**: Communication channels for this interface

### Interface Classes (Examples):
```c
#define USB_CLASS_AUDIO          0x01
#define USB_CLASS_CDC            0x02
#define USB_CLASS_HID            0x03
#define USB_CLASS_MASS_STORAGE   0x08
#define USB_CLASS_HUB            0x09
#define USB_CLASS_MCTP           0x14  // MCTP over USB
```

### Interface Descriptor:
```c
struct usb_interface_descriptor {
    uint8_t bLength;            // Descriptor size
    uint8_t bDescriptorType;    // Interface descriptor type
    uint8_t bInterfaceNumber;   // Interface ID
    uint8_t bAlternateSetting;  // Alternate setting ID
    uint8_t bNumEndpoints;      // Number of endpoints (excluding EP0)
    uint8_t bInterfaceClass;    // Interface class code
    uint8_t bInterfaceSubClass; // Interface subclass code
    uint8_t bInterfaceProtocol; // Interface protocol code
    uint8_t iInterface;         // Interface string index
};
```

### Multi-Interface Device Example:
```
USB Device (ID 0955:cf11)
├── Interface 0: MCTP Communication
│   ├── Class: 0x14 (MCTP)
│   ├── Driver: mctp-usb
│   ├── Endpoint 1 OUT (Bulk)
│   └── Endpoint 1 IN (Bulk)
└── Interface 1: Human Interface Device
    ├── Class: 0x03 (HID)
    ├── Driver: usbhid
    └── Endpoint 2 IN (Interrupt)
```

---

## USB Endpoints

**Endpoints** are **communication channels** or **buffers** within an interface for actual data transfer.

### Endpoint Characteristics:
- **Address**: 4-bit number (0-15) + direction bit
- **Direction**: IN (device→host) or OUT (host→device)
- **Transfer Type**: Control, Bulk, Interrupt, or Isochronous
- **Maximum Packet Size**: Bytes per transfer
- **Interval**: For interrupt/isochronous endpoints

### Endpoint Addressing:
```c
// Endpoint address format: DDDD NNNN
// D = Direction bit (0=OUT, 1=IN)
// N = Endpoint number (0-15)

Endpoint 0 Control = 0x00 (OUT) / 0x80 (IN)
Endpoint 1 Bulk OUT = 0x01
Endpoint 1 Bulk IN = 0x81
Endpoint 2 Interrupt IN = 0x82
```

### Endpoint Types:

#### 1. **Control Endpoints**
- **Purpose**: Device configuration and management
- **Endpoint**: Always endpoint 0 (shared by all interfaces)
- **Direction**: Bidirectional
- **Usage**: Enumeration, descriptor requests, status

#### 2. **Bulk Endpoints** (Used in MCTP)
- **Purpose**: Large data transfers
- **Characteristics**: Reliable delivery, no timing guarantee
- **Usage**: File transfers, MCTP packets
- **Example**: MCTP command/response data

#### 3. **Interrupt Endpoints**
- **Purpose**: Small, periodic data transfers
- **Characteristics**: Guaranteed timing, limited size
- **Usage**: Keyboard, mouse, status updates

#### 4. **Isochronous Endpoints**
- **Purpose**: Time-sensitive streaming data
- **Characteristics**: Guaranteed bandwidth, no error correction
- **Usage**: Audio, video streaming

### Endpoint Descriptor:
```c
struct usb_endpoint_descriptor {
    uint8_t  bLength;          // Descriptor size
    uint8_t  bDescriptorType;  // Endpoint descriptor type
    uint8_t  bEndpointAddress; // Endpoint address and direction
    uint8_t  bmAttributes;     // Transfer type and attributes
    uint16_t wMaxPacketSize;   // Maximum packet size
    uint8_t  bInterval;        // Polling interval
};
```

---

## USB Host vs Device

### USB Host (Your Linux System)
**Role**: Controller/Master
**Responsibilities**:
- Device enumeration and configuration
- Initiating all USB transactions
- Bandwidth allocation and scheduling
- Power management
- Error handling and recovery

**Examples**:
- Desktop computers, laptops
- Embedded Linux systems
- USB hubs
- Smartphones (in host mode)

### USB Device (MCTP Hardware)
**Role**: Peripheral/Slave
**Responsibilities**:
- Responding to host requests
- Providing device descriptors
- Implementing functional interfaces
- Processing received data

**Examples**:
- USB flash drives, keyboards, mice
- MCTP devices, printers, cameras
- Smartphones (in device mode)

### Communication Flow:
```
1. Device Connection:
   Host detects device insertion
   ↓
   Host requests device descriptor (via Endpoint 0)
   ↓
   Device responds with descriptor
   ↓
   Host configures device
   ↓
   Host loads appropriate driver

2. Data Transfer:
   Host prepares data transfer
   ↓
   Host sends data via bulk OUT endpoint
   ↓
   Device processes data
   ↓
   Device sends response via bulk IN endpoint
   ↓
   Host receives response
```

---

## MCTP USB Implementation

### MCTP Device Structure:
```
MCTP USB Device (ID 0955:cf11)
└── Configuration 1
    └── Interface 0 (MCTP Interface)
        ├── Interface Class: 0x14 (MCTP)
        ├── Interface Subclass: 0x00
        ├── Interface Protocol: 0x01
        ├── Driver: mctp-usb (kernel)
        ├── Endpoint 1 OUT (0x01) - Host sends MCTP packets
        └── Endpoint 1 IN  (0x81) - Device sends responses
```

### MCTP USB Header Format:
```c
struct mctp_usb_header {
    uint16_t dmtf_id;    // DMTF ID (0x0001 for standard, 0xB41A for libmctp)
    uint8_t  reserved;   // Reserved byte
    uint8_t  length;     // Total packet length including header
} __attribute__((packed));
```

### Data Flow:
```
Application
    ↓
Network Stack (mctpusb0 interface)
    ↓
Kernel MCTP USB Driver
    ↓
USB Bulk OUT Endpoint → MCTP Device
    ↓
MCTP Device Processing
    ↓
USB Bulk IN Endpoint ← MCTP Device
    ↓
Kernel MCTP USB Driver
    ↓
Network Stack
    ↓
Application
```

### Your Implementation Architecture:
```
1. Kernel Driver (mctp-usb.c):
   - Creates mctpusb* network interfaces
   - Handles data transfer (bulk endpoints)
   - Integrates with Linux network stack

2. Userspace Control (mctp-ctrl-usb.c):
   - Uses libusb for device detection
   - Handles hotplug events
   - Manages MCTP protocol logic
   - Updates D-Bus state

3. Alternative libusb Implementation (usb.c):
   - Pure userspace implementation
   - Direct USB communication
   - Custom protocol handling
```

---

## Port Paths and Device Identification

### USB Port Path Format:
```
<bus>-<hub_port>.<device_port>.<sub_port>...
```

### Your System Example:
```bash
# lsusb -tv output:
Bus 001.Port 001: Dev 001, Class=root_hub
    |__ Port 001: Dev 002, If 0, Class=[unknown], Driver=hub
        |__ Port 001: Dev 003, If 0, Class=[unknown], Driver=mctp-usb
            ID 0955:cf11  # Port path: 1-1.1
        |__ Port 002: Dev 004, If 0, Class=[unknown], Driver=mctp-usb
            ID 0955:ffff  # Port path: 1-1.2
        |__ Port 003: Dev 005, If 0, Class=[unknown], Driver=mctp-usb
            ID 0955:cf11  # Port path: 1-1.3
```

### Service Mapping:
```
Port Path    Device       Service Name
1-1.1    →   Dev 003   →  mctp-usb-ctrl@1-1-1.service
1-1.2    →   Dev 004   →  mctp-usb-ctrl@1-1-2.service  
1-1.3    →   Dev 005   →  mctp-usb-ctrl@1-1-3.service
```

### udev Rule Processing:
```bash
# Original kernel name: 1-1.1:1.0
# udev transformation:
# 1. Replace dots with dashes: 1-1-1:1.0
# 2. Remove interface part: 1-1-1
# Result: mctp-usb-ctrl@1-1-1.service
```

---

## Code Examples

### 1. Device Enumeration (libusb):
```c
// From usb.c - MCTP interface discovery
for (uint8_t j = 0; j < config->bNumInterfaces; ++j) {
    const struct libusb_interface *itf = &config->interface[j];
    
    for (uint8_t k = 0; k < itf->num_altsetting; ++k) {
        const struct libusb_interface_descriptor *itf_desc = 
            &itf->altsetting[k];
            
        // Look for MCTP interface
        if (itf_desc->bInterfaceClass == MCTP_CLASS_ID) {
            // Found MCTP interface - now find endpoints
            for (uint8_t l = 0; l < itf_desc->bNumEndpoints; l++) {
                const struct libusb_endpoint_descriptor *ep_desc = 
                    &itf_desc->endpoint[l];
                    
                // Identify endpoint direction
                if ((ep_desc->bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_OUT)
                    usb->endpoint_out_addr = ep_desc->bEndpointAddress;
                    
                if ((ep_desc->bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_IN)
                    usb->endpoint_in_addr = ep_desc->bEndpointAddress;
            }
        }
    }
}
```

### 2. Kernel Driver Matching:
```c
// From mctp-usb.c - kernel driver device matching
static const struct usb_device_id mctp_usb_devices[] = {
    { USB_INTERFACE_INFO(USB_CLASS_MCTP, 0x0, 0x1) },
    { 0 },
};

// This matches:
// - Interface Class: 0x14 (MCTP)
// - Interface Subclass: 0x0
// - Interface Protocol: 0x1
```

### 3. Data Transfer:
```c
// Bulk transfer setup
libusb_fill_bulk_transfer(
    transfer,                    // Transfer structure
    device_handle,              // Device handle
    endpoint_address,           // Endpoint (e.g., 0x01 for OUT, 0x81 for IN)
    buffer,                     // Data buffer
    length,                     // Data length
    callback_function,          // Completion callback
    user_data,                  // User data for callback
    timeout                     // Timeout (0 = no timeout)
);

// Submit transfer
int result = libusb_submit_transfer(transfer);
```

### 4. Port Path Discovery:
```c
// Get device port path
uint8_t bus_id = libusb_get_bus_number(device);
uint8_t port_numbers[8];
int port_count = libusb_get_port_numbers(device, port_numbers, sizeof(port_numbers));

// Build port path string
char port_path[32];
snprintf(port_path, sizeof(port_path), "%d", bus_id);
for (int i = 0; i < port_count; i++) {
    char port_str[8];
    snprintf(port_str, sizeof(port_str), "-%d", port_numbers[i]);
    strcat(port_path, port_str);
}
// Result: "1-1-3" for bus 1, hub port 1, device port 3
```

---

## Troubleshooting

### Common Issues:

#### 1. **Device Not Detected**
```bash
# Check if device is visible to system
lsusb
lsusb -tv

# Check kernel messages
dmesg | grep -i usb
dmesg | grep -i mctp

# Check udev events
udevadm monitor --environment --udev
```

#### 2. **Driver Not Loading**
```bash
# Check if mctp-usb driver is loaded
lsmod | grep mctp

# Check driver binding
ls /sys/bus/usb/drivers/mctp-usb/

# Manual driver bind (if needed)
echo "1-1.3:1.0" > /sys/bus/usb/drivers/mctp-usb/bind
```

#### 3. **Service Not Starting**
```bash
# Check service status
systemctl status mctp-usb-ctrl@1-1-3.service

# Check service logs
journalctl -u mctp-usb-ctrl@1-1-3.service -f

# Check if udev rule fired
udevadm test /sys/bus/usb/devices/1-1.3:1.0
```

#### 4. **Network Interface Issues**
```bash
# Check if mctpusb interface created
ip link show | grep mctpusb

# Check interface status
ip link show mctpusb0

# Bring interface up manually
ip link set mctpusb0 up
```

#### 5. **Permission Issues**
```bash
# Check USB device permissions
ls -la /dev/bus/usb/001/003

# Add user to groups (if needed)
sudo usermod -a -G dialout,plugdev $USER

# Check libusb access
# May need udev rules for device access
```

### Debug Commands:
```bash
# Detailed device information
lsusb -v -d 0955:cf11

# Interface details
cat /sys/bus/usb/devices/1-1.3:1.0/bInterfaceClass
cat /sys/bus/usb/devices/1-1.3:1.0/bInterfaceSubClass

# Endpoint information
ls /sys/bus/usb/devices/1-1.3:1.0/ep_*

# Driver information
cat /sys/bus/usb/devices/1-1.3:1.0/driver/module/refcnt
```

---

## Summary

This guide covers the essential USB concepts needed to understand your MCTP USB implementation:

1. **USB Device Hierarchy**: Device → Configuration → Interface → Endpoint
2. **Interface Classes**: MCTP uses class 0x14
3. **Endpoints**: Bulk IN/OUT for MCTP data transfer
4. **Port Paths**: Physical topology-based device identification
5. **Driver Architecture**: Kernel driver for data, libusb for control
6. **Service Management**: systemd services per device instance

The combination of kernel drivers, udev rules, and userspace control provides a robust MCTP over USB implementation that handles device hotplug, multi-device scenarios, and persistent device identification.
