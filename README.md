# Embedded NMEA 0183 Parser

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![GitHub top language](https://img.shields.io/github/languages/top/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183)
[![GitHub issues](https://img.shields.io/github/issues/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183/pulls)

## Overview

Embedded NMEA 0183 Parser is a streamlined header-only C library designed for integrating NMEA sentence parsing into embedded systems.

It features a POSIX-like interface (open, close, read, write), which makes it easy to adopt and extend.

## Features

- **POSIX-like Interface:** Use familiar functions to open, close, read from, and write to the parser.
- **Modular Design:** Sentence-specific parsing functions are dispatched cleanly for easy expansion.
- **Configurable Output:** Choose between raw binary data or a JSON-formatted output through simple runtime configuration.
- **Single Header Implementation:** Simplifies integration.
- **Compile Time Sentence Support:** Turn off the sentences you won't use to minimise the size of your binary.
- **Testable and Reliable:** Built for straightforward unit testing and fuzzing.

For a detailed list of supported NMEA sentence types, please refer to [Supported Sentences](doc/supported-sentences.md).

## Getting Started

### Installation

Clone the repository:

```bash
git clone https://github.com/FinOrr/embedded-nmea-0183.git
```

Simply include the header in your source code:

```c
#include "nmea_parser.h"
```

### Example Usage

By default, all NMEA data is accessible through the `g_nmea_state` structure.

If you'd rather have the parser return a JSON for external processing, ensure you first configure the parser:

```C
// Optional: Set output mode to JSON.
g_nmea_config.output_mode = NMEA_OUTPUT_JSON;

// Initialise the parser with the address of the config.
nmea_open(&g_nmea_config);

// Feed NMEA-0183 string into the parser.
const char *your_nmea_string = "$GPGGA,123519,4807.038,N,01131.000,E,...*CC";
nmea_write(your_nmea_string, strlen(your_nmea_string));

// Access parsed data; note that latitude is a string.
const char *current_latitude_str = g_nmea_state.latitude;

// Allocate a buffer for the JSON output.
char json_buffer[256];
ssize_t ret = nmea_read(json_buffer, sizeof(json_buffer));
if(ret > 0) {
    printf("Parsed JSON: %s\n", json_buffer);
}
```

## Configuring Supported Sentences

Sentences can be omitted from compilation if you know your firmware does not require them. This reduces the size of your final binary.

```C
// nmea_parser.h

// To enable a sentence, simply set the config to true
#define CFG_SENTENCE_ENABLED_AAM true
// To disable a sentence, set the config to false
#define CFG_SENTENCE_ENABLED_ABK false
```

## Documentation

For in-depth technical details and a full explanation of the design requirements, please see the [Requirements Document](help/requirements-doc.md).

## License

This project is licensed under the permissive MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For inquiries or support, please open an issue on [GitHub Issues](https://github.com/FinOrr/embedded-nmea-0183/issues).
