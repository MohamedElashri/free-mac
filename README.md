# Free Command for macOS

## Overview
The `free` command is a well-known utility in Linux environments, providing a quick overview of the system's memory usage, including total, used, and free memory, along with swap space information. However, macOS lacks this tool natively. The `free` tool for macOS aims to bridge this gap, offering macOS users a similar functionality to monitor their system's memory usage directly from the command line.

## Features
- **Memory Usage Overview**: Displays total, used, free, cached, app, and wired memory.
- **Swap Space Usage**: Provides details on total, used, and free swap space.
- **Human-Readable Format**: Outputs memory statistics in an easily understandable format.

## Installation

### Homebrew
The `free` tool for macOS is available via `Homebrew` This is the easisest way to use it. Add this tap to your Homebrew

```bash
brew tap MohamedElashri/free-mac
```

Once the tap has been added, you can install free-mac by running:

```bash
brew install free-mac
```

### Build for source

#### Requirements
- GCC compiler or equivalent
- macOS system

#### Compiling
The project includes a Makefile for straightforward compilation. There are two targets:
- `make release` for compiling the tool for production use.
- `make test` for compiling with AddressSanitizer for development and debugging.

##### Release Build
```bash
make release
```

##### Test Build

```bash
make test
```


#### Running the Tool
After compilation, execute the binary from the terminal to see memory statistics:

```bash
./free
```


##### Cleaning Up

To remove the compiled objects and executable:

```bash
make clean
```

#### Example Output
Running the `free` command outputs the memory and swap usage in a formatted table, similar to the following:

```bash
               total           used           free         cached            app          wired
Mem:        16.00 GB       12.77 GB       26.95 MB        3.20 GB        5.15 GB        1.99 GB
Swap:        2.00 GB      359.94 MB        1.65 GB
```



## License

This project is released under the MIT License. See the LICENSE file for more details.

## Contributing

Contributions to the `free-mac` are welcome! Please feel free to submit pull requests or file issues through GitHub to suggest improvements or report bugs.



