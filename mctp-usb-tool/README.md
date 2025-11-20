## Features
- USB port path identification and display


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

### Using BitBake
This package can be built using BitBake as part of the Yocto/OpenEmbedded build system:

```bash
bitbake mctp-usb-tool
