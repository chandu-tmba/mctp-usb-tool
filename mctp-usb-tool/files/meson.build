project('mctp-usb-tool', 'c',
    version: '1.0',
    license: 'Apache-2.0',
    default_options: [
        'c_std=c99',
        'warning_level=3',
        'werror=false',
        'optimization=2',
        'buildtype=release'
    ]
)

# Compiler configuration
cc = meson.get_compiler('c')

# Add compiler flags
add_project_arguments('-D_GNU_SOURCE', language: 'c')

# Dependencies
libusb_dep = dependency('libusb-1.0', required: true)


# Source files
mctp_usb_tool_sources = [
    'src/mctp-usb-tool.c'
]

# Build the executable
dependencies_list = [libusb_dep]

mctp_usb_tool_exe = executable('mctp-usb-tool',
    mctp_usb_tool_sources,
    dependencies: dependencies_list,
    install: true,
    install_dir: get_option('bindir')
)