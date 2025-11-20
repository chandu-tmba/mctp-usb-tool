SUMMARY = "MCTP USB Tool - Utility for MCTP over USB communication (Meson build)"
DESCRIPTION = "A sample C utility that demonstrates MCTP (Management Component Transport Protocol) \
over USB operations including device initialization, message sending and receiving. Built with Meson."
HOMEPAGE = "https://github.com/openbmc/mctp"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

# Version and revision information
PV = "1.0+git${SRCPV}"
PR = "r1"

# Source location - using local files
SRC_URI = "file://src/mctp-usb-tool.c \
           file://meson.build \
          "

# Source directory setup
S = "${WORKDIR}"

# Build dependencies
DEPENDS = "libusb1"

# Runtime dependencies
RDEPENDS:${PN} = "glibc libusb1"

# Inherit meson build class
inherit meson pkgconfig


do_install() {
    # Create destination directory
    install -d ${D}/usr/bin
    
    # Copy executable to image tree
    install -m 0755 mctp-usb-tool ${D}/usr/bin/
}