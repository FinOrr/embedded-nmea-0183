# Embedded NMEA 0183 Parser

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![GitHub top language](https://img.shields.io/github/languages/top/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183)
[![GitHub issues](https://img.shields.io/github/issues/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183/pulls)

## Overview

**Embedded NMEA 0183 Parser** is a C library designed for processing NMEA 0183 sentences on embedded devices. 
It simplifies the handling of NMEA 0183 data, allowing developers to integrate GPS and other navigation-related information into their embedded systems efficiently.

## Features

- NMEA 0183 sentence parsing for embedded devices.
- Lightweight and easy-to-integrate into existing C projects.
- Support for common NMEA sentence types.

## Usage

### Installation

Clone the repository:

```bash
git clone https://github.com/FinOrr/embedded-nmea-0183.git
```

Copy the nmea0183.c and nmea0183.h files into your project directory.

Include the nmea0183.h header file in your source files where you want to use the NMEA 0183 parsing functionality.

```c
#include "nmea0183.h"
```

### Example

See the demo.c program for example usage.
To build the demo, you'll need CMake installed.

Open a terminal, navigate to the directory containing your source files and the CMakeLists.txt, and run the following commands:

```bash
mkdir build
cd build
cmake ..
```

This will generate the build files. Once the files are generated, you can build your project by running:

```bash
make
```

This will compile your source files and create the executable. If you want to clean the generated files, you can run:

```bash
make clean
```

The resulting executable will be in the build directory.

## Documentation

Detailed documentation can be found in the [Wiki](https://github.com/FinOrr/embedded-nmea-0183/wiki).

## Contributing

Contributions are welcome! Please check the [Contribution Guidelines](CONTRIBUTING.md) before making a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contact

For any inquiries or support, please use the [Issues](https://github.com/FinOrr/embedded-nmea-0183/issues) page.
