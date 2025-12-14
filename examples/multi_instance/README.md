# Multi-Instance Example

This example demonstrates using multiple independent NMEA parser contexts simultaneously.

## Use Cases

- Multiple GPS devices (primary + backup)
- Separate GNSS and sensor data streams
- Different data sources requiring different configurations
- Redundant systems for safety-critical applications

## Features

- Three independent parser contexts
- Different module configurations per context
- Comparing data from multiple sources
- Selecting best GPS based on quality metrics

## Building

```bash
# From project root - build all examples
cmake --preset conf-examples && cmake --build --preset build-examples

# Or build manually
gcc -I./inc examples/multi_instance/multi_instance.c -L./out/build/conf-examples/lib -lnmea -lm -o multi_instance
```

## Running

```bash
./out/build/conf-examples/examples/multi_instance
```

## Expected Output

```text
NMEA Multi-Instance Parser Example
===================================

Initializing parser contexts...
  GPS1 context initialized (context size: 216 bytes)
  GPS2 context initialized
  Sensor context initialized

Parsing data from GPS Device 1...
  [OK] $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
  [OK] $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*6A
  [OK] $GPGLL,4807.038,N,01131.000,E,123519,A,A*5C
Parsed 3 sentences from GPS Device 1

=== GPS Device 1 (Primary) ===
Position: 48.117300°, 11.516670°
Altitude: 545.4 m
Speed: 22.4 knots
Course: 84.4°
Fix quality: 1, Satellites: 8
HDOP: 0.9

=== GPS Device 2 (Backup) ===
Position: 48.135000°, 11.541670°
Altitude: 550.0 m
Speed: 18.5 knots
Course: 90.0°
Fix quality: 1, Satellites: 7
HDOP: 1.2

=== Sensor Device ===
Depth: 3.70 m (12.30 ft, 2.00 fathoms)
Water temp: 15.5°C
Wind: 123.4° (T) at 12.5 knots

=== Position Comparison ===
GPS1: 48.117300°, 11.516670°
GPS2: 48.135000°, 11.541670°
Difference: -0.017700° lat, -0.025000° lon
Approximate separation: 3342.9 meters

=== GPS Selection ===
Selected GPS1 (HDOP: 0.9 vs 1.2)
```

## Key Concepts

### 1. Independent Contexts

Each context is completely independent:

```c
nmea_context_t gps1_ctx, gps2_ctx, sensor_ctx;
```

### 2. Different Configurations

Each context can have different enabled modules:

```c
// GPS contexts - only GNSS module
nmea_config_t gps_config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS)
};

// Sensor context - only sensor module
nmea_config_t sensor_config = {
    .enabled_modules = (1 << NMEA_MODULE_SENSOR)
};
```

### 3. Selective Parsing

Sentences are automatically filtered based on configuration:
- GPS contexts ignore sensor sentences
- Sensor context ignores GPS sentences

### 4. Data Comparison

Compare data from multiple sources:

```c
nmea_gnss_state_t gps1_data, gps2_data;
nmea_get_gnss_data(&gps1_ctx, &gps1_data);
nmea_get_gnss_data(&gps2_ctx, &gps2_data);

// Compare positions, accuracy, etc.
```

## Real-World Applications

### Redundant GPS Systems

Use multiple GPS receivers for safety-critical navigation:

```c
// Select GPS with best quality
if (gps1_data.dop.hdop < gps2_data.dop.hdop) {
    use_gps(&gps1_data);
} else {
    use_gps(&gps2_data);
}
```

### Separate Data Streams

Parse NMEA-0183 from different sources:

```c
// Port 1: GPS receiver
process_uart1_data(&gps_ctx);

// Port 2: Depth/wind sensors
process_uart2_data(&sensor_ctx);
```

### Multi-Protocol Integration

Combine different NMEA sources:

```c
// Context 1: Standard GPS (GP talker)
// Context 2: AIS receiver (AI talker)
// Context 3: Instruments (II, SD talkers)
```

## Memory Considerations

Each context consumes:
- ~200-300 bytes for GNSS only
- ~500-800 bytes for GNSS + sensors
- ~1-2 KB for all modules

**Example:** 3 contexts with GNSS + sensors = ~1.5-2.4 KB RAM

## Thread Safety

Contexts are independent but not thread-safe. For multi-threaded use:

```c
// Option 1: Separate contexts per thread (preferred)
pthread_create(&thread1, NULL, gps1_thread, &gps1_ctx);
pthread_create(&thread2, NULL, gps2_thread, &gps2_ctx);

// Option 2: Mutex protection
pthread_mutex_lock(&ctx_mutex);
nmea_parse(&shared_ctx, ...);
pthread_mutex_unlock(&ctx_mutex);
```

## Related Documentation

- [Integration Guide](../../doc/integration-guide.md)
- [Memory Optimization](../../doc/memory-optimization.md)
