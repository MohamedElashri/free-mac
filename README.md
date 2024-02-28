# Free Command for macOS

## Overview
The `free` command is a well-known utility in Linux environments, providing a quick overview of the system's memory usage, including total, used, and free memory, along with swap space information. However, macOS lacks this tool natively. The `free` tool for macOS aims to bridge this gap, offering macOS users a similar functionality to monitor their system's memory usage directly from the command line.

## Motivation
The primary motivation for developing the `free` tool for macOS was the absence of a straightforward, command-line utility in macOS for quickly assessing memory and swap usage. This tool is designed to replicate the functionality of the Linux `free` command, catering to developers, system administrators, and power users transitioning from Linux to macOS or those who work across both platforms.

## Features
- **Memory Usage Overview**: Displays total, used, free, cached, app, and wired memory.
- **Swap Space Usage**: Provides details on total, used, and free swap space.
- **Human-Readable Format**: Outputs memory statistics in an easily understandable format.

## Installation
The `free` tool for macOS is currently being prepared for distribution. Installation instructions will be provided once the tool is available for download.

## Compilation and Usage

### Requirements
- GCC compiler or equivalent
- macOS system

### Compiling
The project includes a Makefile for straightforward compilation. There are two targets:
- `make release` for compiling the tool for production use.
- `make test` for compiling with AddressSanitizer for development and debugging.

#### Release Build
```bash
make release
```

#### Test Build

```bash
make test
```


### Running the Tool
After compilation, execute the binary from the terminal to see memory statistics:

```bash
./free
```

#### Example Output
Running the `free` command outputs the memory and swap usage in a formatted table, similar to the following:

```bash
               total           used           free         cached            app          wired
Mem:        16.00 GB       12.77 GB       26.95 MB        3.20 GB        5.15 GB        1.99 GB
Swap:        2.00 GB      359.94 MB        1.65 GB
```

#### Cleaning Up

To remove the compiled objects and executable:

```bash
make clean
```

## License

This project is released under the MIT License. See the LICENSE file for more details.


## Future Plans

**Homebrew Package**: Efforts are underway to make the free tool available via `Homebrew` for effortless installation on `macOS`.



