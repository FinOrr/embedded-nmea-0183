# NMEA Parser POSIX-like Interface Requirements Document

## Overview

This document outlines the requirements for a lightweight NMEA parser that is implemented as a single header file in C. The parser provides a POSIX-like interface (`open()`, `close()`, `read()`, `write()`) and is designed to be modular, easily expandable, and straightforward to test with unit tests (e.g., using gtest) and fuzzing frameworks.

## Requirements and How They Are Met

### 1. POSIX-like open/close/read/write Interface

- **Requirement:**  
  Provide a familiar device-like interface with functions for opening, closing, reading from, and writing to the parser.
- **How It's Met:**
  - **`nmea_open()`:** Initialises the parser, resets the global state, and sets configuration options (such as output format).
  - **`nmea_close()`:** Cleans up any resources and finalises state (even if currently minimal, it supports future extensions).
  - **`nmea_write()`:** Accepts raw NMEA sentence strings, validates and tokenises them, and dispatches them to the correct sentence-specific parser.
  - **`nmea_read()`:** Returns the latest parsed data in a format configured at initialisation (either as a raw binary copy of the global state or as a JSON-formatted string).

### 2. Function Dispatchers within the Read and Write Callbacks

- **Requirement:**  
  Use dispatchers to delegate the processing of different sentence types to dedicated parsing functions.
- **How It's Met:**
  - The `nmea_write()` function tokenises the incoming string and extracts the sentence identifier.
  - A dispatch table (`g_dispatch_table`) maps sentence identifiers (e.g., `"GGA"`, `"RMC"`) to corresponding parser functions (e.g., `nmea_parse_gga()`, `nmea_parse_rmc()`).
  - This design keeps the main callbacks clean and encapsulates sentence-specific logic in separate, testable functions.

### 3. Configurable Output Format (Raw or JSON)

- **Requirement:**  
  Allow the user to configure the output format—returning either a binary copy of the global state or a JSON string representation.
- **How It's Met:**
  - The configuration structure (`nmea_config_t`) provided to `nmea_open()` lets the caller specify the desired output mode (`NMEA_OUTPUT_RAW` or `NMEA_OUTPUT_JSON`).
  - The `nmea_read()` function checks the global configuration (`g_nmea_config`) and accordingly returns either:
    - A binary copy of the `nmea_global_t` global structure (raw mode), or
    - A JSON-formatted string generated using `snprintf()` (JSON mode).
  - Configuring the output at open-time ensures that the read() function always returns data in the expected format.

### 4. Expandability for New Sentence Types

- **Requirement:**  
  The parser should be easily expandable to support additional NMEA sentence types in the future.
- **How It's Met:**
  - The dispatch table architecture enables new sentence parsers to be added by implementing a new parsing function and inserting an entry in the `g_dispatch_table`.
  - This modular design isolates sentence-specific parsing, minimising the impact on the rest of the system when new sentences are added.

### 5. Single Header Implementation

- **Requirement:**  
  The entire solution must reside within a single header file to simplify integration and reuse across different targets.
- **How It's Met:**
  - All API functions (`nmea_open()`, `nmea_close()`, `nmea_read()`, and `nmea_write()`), helper functions, configuration structures, and the dispatch table are implemented as `static inline` within one header file (`nmea_parser.h`).
  - This reduces external dependencies and simplifies the build process on various platforms.

### 6. Ease of Testing (Unit Tests and Fuzzing)

- **Requirement:**  
  The design should facilitate easy unit testing and fuzzing.
- **How It's Met:**
  - The clear separation of concerns (validation, tokenisation, dispatching, and state updates) allows individual components to be tested in isolation.
  - The global state (`g_nmea_state`) can be reset or inspected easily during tests.
  - Since all functions are defined as inline in a single header file, test harnesses like gtest or fuzzing frameworks can include the header and invoke the functions directly without complex linking or runtime dependencies.

## Conclusion

This design meets the key requirements for a lightweight, POSIX-like NMEA parser with:

- A standardised open/close/read/write interface,
- Clean, modular dispatching to sentence-specific parsing functions,
- Configurable output (raw binary or JSON) set at initialisation,
- A highly expandable architecture to accommodate future sentence types,
- Implementation in a single header for portability and ease-of-use,
- A structure that is inherently testable with unit tests and fuzzing frameworks.

The approach provides a robust foundation for integrating NMEA parsing into embedded systems while ensuring that the system remains flexible and maintainable as requirements evolve.
