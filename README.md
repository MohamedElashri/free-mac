[![Compiling and testing CI](https://github.com/MohamedElashri/free-mac/actions/workflows/compile.yml/badge.svg)](https://github.com/MohamedElashri/free-mac/actions/workflows/compile.yml)

# Free Command for macOS

## Overview
The `free` command is a well-known utility in Linux environments, providing a quick overview of the system's memory usage, including total, used, and free memory, along with swap space information. However, macOS lacks this tool natively. The `free` tool for macOS aims to bridge this gap, offering macOS users a similar functionality to monitor their system's memory usage directly from the command line.

## Installation

### Homebrew
The free tool for macOS is available via Homebrew. This is the easiest way to use it. Add this tap to your Homebrew:
```bash
brew tap MohamedElashri/free-mac
```
Once the tap has been added, you can install `free-mac` by running:
```bash
brew install free-mac
```

### Build from Source

#### Requirements
- GCC compiler or equivalent
- macOS system
  
#### Compiling
The project includes a Makefile for straightforward compilation. There are two targets:
- `make` for compiling the tool for production use.
- `make test` for compiling with AddressSanitizer for development and debugging.

##### Release Build
```bash
make
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
Running the free command outputs the memory and swap usage in a formatted table, similar to the following:
```
               total        used        free      shared  buff/cache   available
Mem:       16.00 GB    12.77 GB    26.95 MB     0.00 B     3.20 GB     4.00 GB
Swap:       2.00 GB   359.94 MB     1.65 GB
```

With total flag (`-t`):
```
               total        used        free      shared  buff/cache   available
Mem:       16.00 GB    12.77 GB    26.95 MB     0.00 B     3.20 GB     4.00 GB
Swap:       2.00 GB   359.94 MB     1.65 GB
Total:     18.00 GB    13.13 GB     1.68 GB
```

## Man Page

### NAME
free - display amount of free and used memory in the system

### SYNOPSIS
```bash
free [OPTIONS]
```

### DESCRIPTION
The free command provides information about the total amount of physical and swap memory in the system, as well as the free and used memory.

### OPTIONS
- `-b, --bytes`: Display the amount of memory in bytes.
- `-k, --kibi`: Display the amount of memory in kibibytes. This is the default.
- `-m, --mebi`: Display the amount of memory in mebibytes.
- `-g, --gibi`: Display the amount of memory in gibibytes.
- `--tebi`: Display the amount of memory in tebibytes.
- `--pebi`: Display the amount of memory in pebibytes.
- `--kilo`: Display the amount of memory in kilobytes. Implies --si.
- `--mega`: Display the amount of memory in megabytes. Implies --si.
- `--giga`: Display the amount of memory in gigabytes. Implies --si.
- `--tera`: Display the amount of memory in terabytes. Implies --si.
- `--peta`: Display the amount of memory in petabytes. Implies --si.
- `-h, --human`: Show all output fields automatically scaled to shortest three digit unit and display the units.
- `-w, --wide`: Switch to the wide mode. The wide mode produces lines longer than 80 characters.
- `-c, --count count`: Display the result count times. Requires the -s option.
- `-l, --lohi`: Show detailed low and high memory statistics.
- `-L, --line`: Show output on a single line, often used with the -s option to show memory statistics repeatedly.
- `-s, --seconds delay`: Continuously display the result delay seconds apart.
- `--si`: Use kilo, mega, giga etc (power of 1000) instead of kibi, mebi, gibi (power of 1024).
- `-t, --total`: Display a line showing the column totals.
- `-v, --committed`: Display a line showing the memory commit limit and amount of committed memory.
- `-d, --debug`: Enable debug output.
- `--help`: Print help.
- `-V, --version`: Display version information.

## License
This project is released under the MIT License. See the LICENSE file for more details.

## Contributing
Contributions to the free-mac are welcome! Please feel free to submit pull requests or file issues through GitHub to suggest improvements or report bugs.