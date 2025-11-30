# Marine Instruments Example

**Use case:** Sailboats, motor yachts, legacy marine electronics integration

**Complexity:** Intermediate

**Memory:** ~600 bytes RAM, ~15 KB flash

## What This Example Does

This example shows how to integrate traditional marine instruments that use NMEA-0183:

- **Depth sounders** - Water depth below keel
- **Water temperature sensors** - Sea temperature
- **Wind instruments** - Wind speed and direction
- **Compass/heading sensors** - True and magnetic heading
- **Speed logs** - Speed through water
- **Rate of turn indicators** - How fast the vessel is turning
- **Distance logs** - Cumulative distance traveled

Demonstrates displaying boat instruments on a single screen or integrating legacy equipment into modern systems.

## What You'll See

```
╔═══════════════════════════════════════╗
║  Marine Instruments Example           ║
║  Legacy NMEA-0183 Sensor Integration  ║
╚═══════════════════════════════════════╝

Parser initialized (Sensor + Heading modules)
Memory usage: 624 bytes

═══════════════════════════════════════
 Parsing instrument data...
═══════════════════════════════════════
  [OK] DBT parsed
  [OK] MTW parsed
  [OK] MWD parsed
  [OK] MWV parsed
  [OK] HDT parsed
  [OK] HDG parsed
  [OK] ROT parsed
  [OK] VHW parsed
  [OK] VBW parsed
  [OK] VLW parsed
  [OK] DPT parsed

Parsed 11 sentences successfully

═══════════════════════════════════════
 Instrument Readings
═══════════════════════════════════════

┌─ DEPTH SOUNDER ─────────────────┐
│ Depth:      3.7 m ( 12.3 ft)   │
│             2.0 fathoms        │
│ Offset:     0.5 m             │
│ Max depth: 100.0 m             │
└─────────────────────────────────┘

┌─ WATER TEMPERATURE ─────────────┐
│ Temperature:  15.5°C            │
│               59.9°F            │
└─────────────────────────────────┘

┌─ WIND INSTRUMENTS ──────────────┐
│ Direction:    45.0° True       │
│               48.5° Magnetic   │
│ Speed:        15.2 knots       │
│                7.8 m/s         │
│               28.2 km/h        │
│ Reference:   Relative          │
└─────────────────────────────────┘

┌─ HEADING/COMPASS ───────────────┐
│ True heading:       234.5°    │
│ Magnetic heading:   231.0°    │
│ Variation:            3.5° E  │
└─────────────────────────────────┘

┌─ RATE OF TURN ──────────────────┐
│ ROT:    +2.5°/min             │
│       Turning right             │
└─────────────────────────────────┘

┌─ SPEED THROUGH WATER ───────────┐
│ Speed:   6.5 knots            │
│         12.0 km/h             │
│ SOG:     7.2 knots (GPS)      │
└─────────────────────────────────┘

┌─ DISTANCE LOG ──────────────────┐
│ Through water:   1234.5 nm     │
│ Over ground:      987.6 nm     │
└─────────────────────────────────┘

═══════════════════════════════════════
 Navigation Summary
═══════════════════════════════════════

 Current course: 234.5° True
 Speed:          6.5 knots
 Depth:          3.7 m
 Wind:           45° at 15.2 knots
```

## How It Works

### 1. Setup for Marine Instruments

Enable the sensor and heading modules:

```c
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_SENSOR) |   // Depth, wind, temp
                      (1 << NMEA_MODULE_HEADING),    // Compass, ROT
    .validate_checksums = false  // Legacy equipment often lacks checksums
};
```

**Why disable checksums?** Many older marine instruments don't include checksums in their NMEA output, or use proprietary formats. Disabling validation allows you to read their data.

### 2. Parse Instrument Data

The library automatically recognizes different instrument types:

```c
nmea_parse(&ctx, sentence, length, buffer, buffer_size);
```

**What happens:** Depth data goes to the sensor module, heading data goes to the heading module, wind data goes to the sensor module, etc. The library organizes everything for you.

### 3. Retrieve Organized Data

Get all sensor readings in one structure:

```c
nmea_sensor_state_t sensors;
nmea_get_sensor_data(&ctx, &sensors);

nmea_heading_state_t heading;
nmea_get_heading_data(&ctx, &heading);
```

**What you get:** All related data is grouped together - all depth readings in one place, all wind data together, etc.

## Understanding Marine Instruments

### Depth Sounder (DBT, DPT sentences)

Measures water depth below the transducer:

```c
sensors.depth_below_transducer_meters  // Depth in meters
sensors.depth_below_transducer_feet    // Depth in feet
sensors.depth_below_transducer_fathoms // Depth in fathoms
sensors.depth_offset_meters            // Keel to transducer distance
```

**Important:** Depth is measured from the transducer, not from the waterline. Use the offset to calculate depth below keel.

**Real depth below keel = depth_below_transducer - depth_offset**

### Water Temperature (MTW sentence)

```c
sensors.water_temperature_celsius  // °C
```

Useful for fishing, detecting currents, or climate monitoring.

### Wind Instruments (MWD, MWV sentences)

```c
sensors.wind_direction_true       // True wind direction (°)
sensors.wind_direction_magnetic   // Magnetic wind direction (°)
sensors.wind_speed_knots          // Wind speed (knots)
sensors.wind_speed_mps            // Wind speed (m/s)
sensors.wind_speed_kmh            // Wind speed (km/h)
sensors.wind_reference            // 'R' = relative, 'T' = true
```

**Apparent vs True Wind:**
- **Apparent (Relative):** Wind felt on the boat (includes boat's motion)
- **True:** Actual wind over the water surface

### Compass/Heading (HDT, HDG, HDM sentences)

```c
heading.heading_true           // True heading (°)
heading.heading_magnetic       // Magnetic heading (°)
heading.variation              // Magnetic variation (°)
heading.variation_direction    // 'E' or 'W'
heading.deviation              // Compass deviation (°)
heading.deviation_direction    // 'E' or 'W'
```

**True vs Magnetic:**
- **True heading:** Direction relative to true north
- **Magnetic heading:** Direction relative to magnetic north
- **Variation:** Difference between true and magnetic north (changes by location)
- **Deviation:** Error in compass caused by metal on boat

### Rate of Turn (ROT sentence)

```c
heading.rate_of_turn  // Degrees per minute
```

- Positive = turning right (starboard)
- Negative = turning left (port)
- Zero = steady course

Useful for autopilot systems and turn coordination.

### Speed Through Water (VHW, VBW sentences)

```c
sensors.water_speed_knots   // Speed relative to water
sensors.speed_ground_knots  // Speed over ground (from GPS)
```

**Important distinction:**
- **Speed through water:** How fast the boat moves through the water (from paddlewheel)
- **Speed over ground (SOG):** How fast the boat moves over the seabed (from GPS)
- **Difference:** Current pushing or slowing the boat

### Distance Log (VLW sentence)

```c
sensors.distance_water_nm   // Cumulative distance through water (nautical miles)
sensors.distance_ground_nm  // Cumulative distance over ground (nautical miles)
```

Like an odometer for your boat. Useful for:
- Tracking voyages
- Calculating fuel consumption
- Maintenance scheduling

## NMEA Sentence Types Explained

### DBT - Depth Below Transducer
```
$SDDBT,12.3,f,3.7,M,2.0,F*3A
       │    │  │   │  │   └─ Fathoms
       │    │  │   │  └─ Unit (F)
       │    │  │   └─ Meters
       │    │  └─ Unit (M)
       │    └─ Feet
       └─ Unit (f)
```

### MTW - Water Temperature
```
$IIMTW,15.5,C*21
       │    └─ Unit (C = Celsius)
       └─ Temperature
```

### MWD - Wind Direction & Speed
```
$IIMWD,045.0,T,048.5,M,15.2,N,7.8,M*52
       │     │  │     │  │    │  │   └─ Unit (M = m/s)
       │     │  │     │  │    │  └─ Speed
       │     │  │     │  │    └─ Unit (N = knots)
       │     │  │     │  └─ Speed
       │     │  │     └─ Reference (M = magnetic)
       │     │  └─ Direction
       │     └─ Reference (T = true)
       └─ Direction
```

### HDT - True Heading
```
$HCHDT,234.5,T*1C
       │     └─ True (T)
       └─ Heading in degrees
```

### ROT - Rate of Turn
```
$HEROT,2.5,A*3F
       │   └─ Valid (A = valid data)
       └─ Rate in degrees/minute
```

## Connecting Real Instruments

Most marine instruments output NMEA-0183 via RS-422 or RS-232:

```
Instrument → RS-422/RS-232 → Level shifter → MCU UART
```

**Typical setup:**
- Baud rate: 4800 (older) or 9600 (newer)
- Some instruments use 38400
- Check your instrument manual

**Wiring:**
- RS-422: Differential (+ and - wires)
- RS-232: Single-ended (TX, RX, GND)
- Use appropriate level shifter for 3.3V or 5V logic

## Use Cases

### 1. Instrument Display Panel

Create a multi-function display showing all instruments:

```c
while (1) {
    // Parse incoming NMEA data
    if (nmea_parse(&ctx, sentence, ...) == NMEA_OK) {
        // Update displays
        update_depth_display(sensors.depth_below_transducer_meters);
        update_speed_display(sensors.water_speed_knots);
        update_heading_display(heading.heading_true);
        update_wind_display(sensors.wind_speed_knots, sensors.wind_direction_true);
    }
}
```

### 2. Anchor Watch Alarm

Alert if boat drags anchor:

```c
float initial_depth = sensors.depth_below_transducer_meters;

// Later...
if (fabs(sensors.depth_below_transducer_meters - initial_depth) > 2.0) {
    sound_alarm();  // Depth changed significantly
}
```

### 3. Shallow Water Warning

```c
float MIN_SAFE_DEPTH = 2.0;  // meters

if (sensors.depth_valid &&
    sensors.depth_below_transducer_meters < MIN_SAFE_DEPTH) {
    show_warning("SHALLOW WATER");
}
```

### 4. Current Calculation

```c
// Difference between speed through water and speed over ground
float current_speed = sensors.speed_ground_knots - sensors.water_speed_knots;

if (current_speed > 0.5) {
    printf("Favorable current: %.1f knots\n", current_speed);
} else if (current_speed < -0.5) {
    printf("Adverse current: %.1f knots\n", -current_speed);
}
```

### 5. Autopilot Data Source

```c
if (heading.heading_true_valid) {
    autopilot_set_heading(heading.heading_true);
}

if (heading.rate_of_turn_valid) {
    autopilot_set_rot_feedback(heading.rate_of_turn);
}
```

## Building and Running

```bash
# From project root - build all examples
cmake --preset conf-examples && cmake --build --preset build-examples
./out/build/conf-examples/examples/marine_instruments

# Or build manually
gcc -I./inc examples/marine_instruments/marine_instruments.c -L./out/build/conf-examples/lib -lnmea -o marine_instruments
./marine_instruments
```

## Talker IDs

NMEA sentences start with a talker ID indicating the source:

- **GP** - GPS receiver
- **II** - Integrated instrumentation
- **SD** - Sounder (depth)
- **HC** - Heading compass
- **HE** - Gyro heading
- **VW** - Speed log
- **WI** - Wind instrument

The library automatically handles all talker IDs.

## Troubleshooting

### "No data from depth sounder"

- Check baud rate (often 4800 on older units)
- Verify wiring (some use RS-422, needs proper termination)
- Disable checksum validation if legacy equipment
- Some depth sounders only transmit when depth changes

### "Wind direction jumping around"

- Normal in light winds
- Check sensor mounting (vibration?)
- Some instruments need damping configuration

### "Heading data incorrect"

- Verify compass calibration/deviation settings
- Check for magnetic interference near sensor
- Ensure variation is set correctly

## Memory Usage

This configuration uses:

- **RAM:** ~600 bytes (sensor + heading modules)
- **Flash:** ~15 KB (code for both modules)
- **Stack:** ~256 bytes (parse buffer)

To reduce memory, disable unused sentence types in `inc/nmea_config.h`.

## Next Steps

**After understanding this example:**

1. **Connect real instruments** → See [serial_port_integration example](../serial_stream/)
2. **Build a dashboard** → Combine with display library (e.g., LCD, OLED)
3. **Add GPS integration** → Combine GNSS, Sensor, and Heading modules
4. **Reduce memory** → See [minimal_memory example](../minimal_memory/)

## Related Documentation

- [Integration Guide](../../doc/integration-guide.md)
- [Serial Port Integration](../../doc/serial-port-integration.md)
- [Supported Sentences](../../doc/supported-sentences.md)
- [Memory Optimization](../../doc/memory-optimization.md)

## Compatible Instruments

This library works with any NMEA-0183 compatible marine instrument:

- **Depth sounders:** Raymarine, Garmin, Furuno, Airmar, etc.
- **Wind instruments:** B&G, Raymarine, NASA, etc.
- **Compasses:** KVH, Airmar, Robertson, etc.
- **Speed logs:** Most paddle-wheel and ultrasonic types
- **Multi-function displays:** As data source or consumer

Any device outputting standard NMEA-0183 sentences is compatible.
