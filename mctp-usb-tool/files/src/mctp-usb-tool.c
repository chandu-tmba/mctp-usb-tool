#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>

/**
 * Simple MCTP USB Tool - Takes vendor ID and device ID as parameters
 */

void print_usage(const char *prog_name) {
    printf("Usage: %s <vendor_id> <device_id>\n", prog_name);
    printf("  vendor_id    USB Vendor ID (hexadecimal, e.g., 0x1234)\n");
    printf("  device_id    USB Device ID (hexadecimal, e.g., 0x5678)\n");
    printf("\nThis tool will:\n");
    printf("  - Find the USB device with the specified VID:PID\n");
    printf("  - Display device information including USB port path\n");
    printf("  - Show bus number and device address\n");
    printf("\nExample:\n");
    printf("  %s 0x1234 0x5678\n", prog_name);
}

void print_port_path_info(libusb_device* dev, uint8_t bus)
{
    uint8_t port_path[8];
    int r, i;

    bus = libusb_get_bus_number(dev);

    r = libusb_get_port_numbers(dev, port_path, sizeof(port_path));
    if (r > 0)
    {
        printf("USB Device Port Path: %d-%d", bus, port_path[0]);
        for (i = 1; i < r; i++)
        {
            printf("-%d", port_path[i]);
        }
    }
    printf("\n");
}

int mctp_usb_init(uint16_t vendor_id, uint16_t device_id) {
    libusb_context *ctx = NULL;
    libusb_device_handle *handle = NULL;
    int ret;

    /* Initialize libusb */
    ret = libusb_init(&ctx);
    if (ret < 0) {
        fprintf(stderr, "Error: Failed to initialize libusb: %s\n", libusb_error_name(ret));
        return ret;
    }

    /* Try to open the device */
    handle = libusb_open_device_with_vid_pid(ctx, vendor_id, device_id);
    if (handle == NULL) {
        fprintf(stderr, "Error: Could not find/open USB device with VID:PID %04x:%04x\n", 
                vendor_id, device_id);
        libusb_exit(ctx);
        return -1;
    }


    /* Get device descriptor for more information */
    libusb_device *device = libusb_get_device(handle);
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(device, &desc);

    /* Get bus and device numbers */
    uint8_t bus_number = libusb_get_bus_number(device);
    print_port_path_info(device, bus_number);

    /* Clean up */
    libusb_close(handle);
    libusb_exit(ctx);
    
    if( desc.bDeviceClass == 0x14 ) {
        printf("Device is a MCTP USB Device\n");
    }
    else
    {
        printf("Device is not MCTP USB Device\n");
    }
    return 0;
}

uint16_t parse_hex_value(const char *str) {
    char *endptr;
    unsigned long value;
    
    /* Handle both 0x prefix and plain hex */
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        value = strtoul(str, &endptr, 16);
    } else {
        value = strtoul(str, &endptr, 16);
    }
    
    if (*endptr != '\0' || value > 0xFFFF) {
        fprintf(stderr, "Error: Invalid hex value '%s'\n", str);
        return 0;
    }
    
    return (uint16_t)value;
}

int main(int argc, char *argv[]) {
    uint16_t vendor_id, device_id;

    /* Check command line arguments */
    if (argc != 3) {
        fprintf(stderr, "Error: Incorrect number of arguments\n\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Parse vendor ID */
    vendor_id = parse_hex_value(argv[1]);
    if (vendor_id == 0 && strcmp(argv[1], "0") != 0 && strcmp(argv[1], "0x0") != 0) {
        return 1;
    }

    /* Parse device ID */
    device_id = parse_hex_value(argv[2]);
    if (device_id == 0 && strcmp(argv[2], "0") != 0 && strcmp(argv[2], "0x0") != 0) {
        return 1;
    }

    /* Initialize and work with the MCTP USB device */
    return mctp_usb_init(vendor_id, device_id);
}