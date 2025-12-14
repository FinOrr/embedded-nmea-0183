# GPS Tracker Example

**Use case:** GPS position tracking, basic location services

**Complexity:** Beginner

**Memory:** ~200 bytes RAM, ~6 KB flash

## Overview

Basic GPS tracking example demonstrating:

- Parsing GPS position (latitude/longitude)
- Retrieving altitude above sea level
- Reading speed and direction of travel
- Extracting UTC time and date
- Checking GPS fix quality and satellite count

## Example Output

```
Simple GPS Tracker Example
===========================

Parser initialized successfully
Memory usage: 216 bytes

Parsing GPS sentences...
  [OK] Sentence 1 parsed
  [OK] Sentence 2 parsed
  [OK] Sentence 3 parsed

=== GPS Information ===

Position:
  Latitude:  48.117300° N
  Longitude: 11.516670° E

Altitude:
  545.4 meters (1789.4 feet)

Speed:
  22.4 knots
  41.5 km/h
  25.8 mph

Course:
  84.4° (true)

Time (UTC):
  12:35:19

Date:
  2025-03-23

Fix Quality:
  Quality: 1 (GPS fix)
  Satellites: 8
  HDOP: 0.9 (Excellent)

Magnetic Variation:
  3.1° W
```

## Implementation

### 1. Parser Setup

Configure the parser for GNSS module only:

```c
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS),
    .validate_checksums = true
};
nmea_init(&ctx, &config);
```

### 2. Sentence Parsing

Parse incoming NMEA sentences:

```c
nmea_parse(&ctx, sentence, length, buffer, buffer_size);
```

The library extracts data from GPS sentences (GGA, RMC, GLL, etc.) and updates internal state.

### 3. Data Retrieval

Access parsed GPS data:

```c
nmea_gnss_state_t gps_data;
nmea_get_gnss_data(&ctx, &gps_data);
```

### 4. Field Access

Use the parsed data:

```c
float latitude = gps_data.latitude.degrees;
float longitude = gps_data.longitude.degrees;
float altitude = gps_data.altitude_meters;
float speed = gps_data.speed.knots;
```

## Building and Running

```bash
# From project root - build all examples
cmake --preset conf-examples && cmake --build --preset build-examples
./out/build/conf-examples/examples/gps_tracker

# Or build manually
gcc -I./inc examples/gps_tracker/gps_tracker.c -L./out/build/conf-examples/lib -lnmea -o gps_tracker
./gps_tracker
```

## Connecting to a Real GPS Module

In a real application, GPS sentences come from a serial port (UART). Here's the minimal code:

```c
// Read from serial port (pseudo-code)
char sentence[128];
read_uart_line(sentence);  // Your UART read function

// Parse it
char parse_buffer[256];
nmea_parse(&ctx, sentence, strlen(sentence), parse_buffer, sizeof(parse_buffer));

// Get data
nmea_gnss_state_t gps;
nmea_get_gnss_data(&ctx, &gps);

// Use position
printf("Lat: %.6f, Lon: %.6f\n", gps.latitude.degrees, gps.longitude.degrees);
```

**For complete serial port examples, see:**
- [serial_stream](../serial_stream/) - POSIX/Linux
- [Serial Port Integration Guide](../../doc/serial-port-integration.md) - STM32, ESP32, Arduino

## Understanding the GPS Data

### Position (Latitude/Longitude)

```c
gps_data.latitude.degrees   // e.g., 48.117300 (positive = North, negative = South)
gps_data.longitude.degrees  // e.g., 11.516670 (positive = East, negative = West)
```

These are decimal degrees - the standard format for GPS coordinates.

### Altitude

```c
gps_data.altitude_meters  // Height above mean sea level
```

### Speed and Course

```c
gps_data.speed.knots  // Speed in nautical miles per hour
gps_data.speed.kmh    // Speed in kilometers per hour
gps_data.speed.mps    // Speed in meters per second
gps_data.course.degrees  // Direction of travel (0-359°)
```

### Time and Date

```c
gps_data.utc_time.hour    // 0-23
gps_data.utc_time.minute  // 0-59
gps_data.utc_time.second  // 0-59
gps_data.utc_date.year    // e.g., 2024
gps_data.utc_date.month   // 1-12
gps_data.utc_date.day     // 1-31
```

GPS provides UTC time (no timezone offset).

### Fix Quality

```c
gps_data.fix_quality
  0 = No fix (position invalid)
  1 = GPS fix (standard)
  2 = DGPS fix (better accuracy)
  4 = RTK fix (centimeter accuracy)
  5 = RTK float (decimeter accuracy)
```

Check `fix_quality > 0` before using position data.

### Satellite Count

```c
gps_data.satellites_used  // Number of satellites in use
```

- 0-3: No valid fix
- 4-5: Minimal fix (use caution)
- 6-8: Good fix
- 9+: Excellent fix

### HDOP (Horizontal Dilution of Precision)

```c
gps_data.dop.hdop  // Lower is better
  < 1.0 = Excellent
  < 2.0 = Good
  < 5.0 = Moderate
  > 5.0 = Poor
```

This tells you how accurate the position is based on satellite geometry.

## Common Use Cases

### Simple Position Display

```c
if (gps_data.latitude.valid && gps_data.longitude.valid) {
    printf("Position: %.6f, %.6f\n",
           gps_data.latitude.degrees,
           gps_data.longitude.degrees);
}
```

### Speed Display

```c
if (gps_data.speed.valid) {
    printf("Speed: %.1f km/h\n", gps_data.speed.kmh);
}
```

### Check if GPS has Fix

```c
if (gps_data.fix_quality > 0 && gps_data.satellites_used >= 4) {
    // Position is valid - safe to use
} else {
    // No fix - show "Searching for GPS..."
}
```

### GPS-Based Clock

```c
if (gps_data.utc_time.valid && gps_data.utc_date.valid) {
    // Set system time from GPS
    set_system_time(gps_data.utc_date.year,
                    gps_data.utc_date.month,
                    gps_data.utc_date.day,
                    gps_data.utc_time.hour,
                    gps_data.utc_time.minute,
                    gps_data.utc_time.second);
}
```

## What GPS Sentences Are Parsed

This example handles these GPS sentence types:

- **GGA** - Position, altitude, fix quality, satellites
- **RMC** - Position, speed, course, date (recommended minimum)
- **GLL** - Position and time
- **VTG** - Course and speed
- **GSA** - Satellite selection and accuracy
- **GSV** - Satellites in view (optional, uses more memory)

You can disable unused sentences in `inc/nmea_config.h` to save memory.

## Troubleshooting

### "No GPS fix"

- GPS needs clear view of sky
- Wait 30-60 seconds for initial fix ("cold start")
- Check antenna connection
- Verify GPS module is powered correctly

### "Position not updating"

- Make sure you're parsing new sentences regularly
- Check UART connection
- Verify baud rate (usually 9600 or 4800)

### "Checksum errors"

- Check serial cable quality
- Verify baud rate matches GPS output
- Check for electrical interference

## Next Steps

After understanding this example:

1. Connect to real GPS hardware - See [serial_stream example](../serial_stream/)
2. Minimize memory usage - See [minimal_memory example](../minimal_memory/)
3. Add error handling - See [error_handling example](../error_handling/)

## Related Documentation

- [Integration Guide](../../doc/integration-guide.md) - Complete setup guide
- [Serial Port Integration](../../doc/serial-port-integration.md) - UART connection
- [FAQ](../../doc/faq.md) - Common questions

## Memory Usage

This configuration uses minimal resources:

- **RAM:** ~200 bytes (context + state)
- **Flash:** ~6 KB (code size)
- **Stack:** ~256 bytes (parse buffer)

Suitable for resource-constrained microcontrollers.
