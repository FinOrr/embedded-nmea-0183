# Serial Stream Example

This example demonstrates parsing NMEA data from a serial port in real-time.

## Files

- `serial_stream_posix.c` - POSIX/Linux/macOS implementation

## Features

- Opens and configures a serial port (UART)
- Buffers incoming data byte-by-byte
- Parses complete NMEA sentences
- Displays GPS position, speed, and satellite info
- Tracks statistics (successful parses, errors, etc.)
- Handles Ctrl+C for clean shutdown

## Building

### Linux/macOS

```bash
# From project root - build all examples
cmake --preset conf-examples && cmake --build --preset build-examples

# Or build manually
gcc -I./inc examples/serial_stream/serial_stream_posix.c -L./out/build/conf-examples/lib -lnmea -o serial_stream
```

## Usage

### Find Your GPS Device

**Linux:**
```bash
# List USB serial devices
ls /dev/ttyUSB*
ls /dev/ttyACM*

# Or use dmesg
dmesg | grep tty
```

**macOS:**
```bash
ls /dev/cu.usbserial*
ls /dev/cu.usbmodem*
```

### Run the Example

```bash
# Linux - typical GPS module
./out/build/conf-examples/examples/serial_stream /dev/ttyUSB0 9600

# Linux - some GPS modules
./out/build/conf-examples/examples/serial_stream /dev/ttyACM0 4800

# macOS
./out/build/conf-examples/examples/serial_stream /dev/cu.usbserial-14230 9600
```

### Permissions (Linux)

If you get a "Permission denied" error:

```bash
# Option 1: Add your user to dialout group
sudo usermod -a -G dialout $USER
# Log out and back in

# Option 2: Run with sudo (not recommended)
sudo ./out/build/conf-examples/examples/serial_stream /dev/ttyUSB0 9600

# Option 3: Change permissions temporarily
sudo chmod 666 /dev/ttyUSB0
```

## Expected Output

```text
Opening serial port: /dev/ttyUSB0 at 9600 baud
Serial port opened successfully
NMEA parser initialized
Waiting for data... (Press Ctrl+C to exit)

--- GPS Data ---
Position: 48.117300° N, 11.516670° E
Altitude: 545.4 m
Time: 12:35:19 UTC
Speed: 22.4 knots (41.5 km/h)
Fix quality: 1
Satellites: 8
HDOP: 0.9

=== Statistics ===
Total bytes:      12456
Total sentences:  123
Successful:       98 (79.7%)
Checksum errors:  2
Invalid:          3
Unknown/Disabled: 20
```

## Troubleshooting

### No data received

- Check device path (`ls /dev/tty*`)
- Verify baud rate (try 4800, 9600, 38400)
- Check USB cable connection
- Try a different USB port

### Garbled data / checksum errors

- Wrong baud rate
- Check parity/stop bits (should be 8N1)
- Cable too long or poor quality
- Electrical interference

### "Device busy" error

Another program is using the serial port:

```bash
# Find the process
lsof /dev/ttyUSB0

# Or
fuser /dev/ttyUSB0
```

## Porting to Other Platforms

### STM32 HAL

See `doc/serial-port-integration.md` for STM32 examples.

### ESP32

Use ESP-IDF UART driver. See `doc/serial-port-integration.md`.

### Arduino

Similar concept, use `Serial.read()` in a loop:

```cpp
if (Serial.available()) {
    char c = Serial.read();
    // Add to buffer
}
```

## Related Documentation

- [Serial Port Integration](../../doc/serial-port-integration.md)
- [Stream Parsing Guide](../../doc/stream-parsing.md)
- [Error Handling](../../doc/error-handling.md)
