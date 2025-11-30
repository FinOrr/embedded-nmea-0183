/**
 * @file gps_tracker.c
 * @brief Simple GPS tracker example - GNSS module only
 *
 * This is the simplest possible example for GPS tracking.
 * Perfect for beginners or projects that only need position data.
 *
 * What this does:
 * - Parses GPS sentences (GGA, RMC, GLL)
 * - Displays: latitude, longitude, altitude, speed, course, time, date
 * - Shows fix quality and satellite count
 * - Minimal memory footprint (~200 bytes RAM, ~6 KB flash)
 */

#include <stdio.h>
#include <string.h>
#include "nmea.h"

int main(void) {
    printf("Simple GPS Tracker Example\n");
    printf("===========================\n\n");

    /* Step 1: Create parser context */
    nmea_context_t ctx;

    /* Step 2: Configure - GNSS module only */
    nmea_config_t config = {
        .enabled_modules = (1 << NMEA_MODULE_GNSS),  /* Only GPS/GNSS */
        .validate_checksums = true,                   /* Validate data integrity */
        .error_callback = NULL                        /* No callback needed for simple usage */
    };

    /* Step 3: Initialize parser */
    if (nmea_init(&ctx, &config) != NMEA_OK) {
        fprintf(stderr, "Failed to initialize parser\n");
        return 1;
    }

    printf("Parser initialized successfully\n");
    printf("Memory usage: %zu bytes\n\n", nmea_get_context_size());

    /* Step 4: Prepare buffer for parsing */
    char parse_buffer[256];

    /* Step 5: Parse GPS sentences */
    /* In a real application, these would come from a GPS module via UART */
    const char *gps_sentences[] = {
        /* GGA: Position, altitude, fix quality */
        "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n",

        /* RMC: Position, speed, course, date */
        "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,100325,003.1,W,A*0D\r\n",

        /* GLL: Position, time */
        "$GPGLL,4807.038,N,01131.000,E,123519,A,A*5C\r\n",

        NULL
    };

    printf("Parsing GPS sentences...\n");
    for (int i = 0; gps_sentences[i] != NULL; i++) {
        nmea_result_t result = nmea_parse(
            &ctx,
            gps_sentences[i],
            strlen(gps_sentences[i]),
            parse_buffer,
            sizeof(parse_buffer)
        );

        if (result == NMEA_OK) {
            printf("  [OK] Sentence %d parsed\n", i + 1);
        } else {
            printf("  [ERR] Sentence %d failed: %s\n", i + 1,
                   nmea_get_error_string(result));
        }
    }

    /* Step 6: Get GPS data */
    nmea_gnss_state_t gps_data;

    if (nmea_get_gnss_data(&ctx, &gps_data) != NMEA_OK) {
        fprintf(stderr, "Failed to get GPS data\n");
        return 1;
    }

    /* Step 7: Display GPS information */
    printf("\n=== GPS Information ===\n");

    /* Position */
    if (gps_data.latitude.valid && gps_data.longitude.valid) {
        printf("\nPosition:\n");
        printf("  Latitude:  %.6f° %c\n",
               fabs(gps_data.latitude.degrees),
               gps_data.latitude.degrees >= 0 ? 'N' : 'S');
        printf("  Longitude: %.6f° %c\n",
               fabs(gps_data.longitude.degrees),
               gps_data.longitude.degrees >= 0 ? 'E' : 'W');
    } else {
        printf("\nPosition: NO FIX\n");
    }

    /* Altitude */
    if (gps_data.altitude_valid) {
        printf("\nAltitude:\n");
        printf("  %.1f meters (%.1f feet)\n",
               gps_data.altitude_meters,
               gps_data.altitude_meters * 3.28084f);
    }

    /* Speed */
    if (gps_data.speed.valid) {
        printf("\nSpeed:\n");
        printf("  %.1f knots\n", gps_data.speed.knots);
        printf("  %.1f km/h\n", gps_data.speed.kmh);
        printf("  %.1f mph\n", gps_data.speed.knots * 1.15078f);
    }

    /* Course (heading) */
    if (gps_data.course.valid) {
        printf("\nCourse:\n");
        printf("  %.1f° (%s)\n",
               gps_data.course.degrees,
               gps_data.course.is_true ? "true" : "magnetic");
    }

    /* Time */
    if (gps_data.utc_time.valid) {
        printf("\nTime (UTC):\n");
        printf("  %02d:%02d:%02d\n",
               gps_data.utc_time.hour,
               gps_data.utc_time.minute,
               gps_data.utc_time.second);
    }

    /* Date */
    if (gps_data.utc_date.valid) {
        printf("\nDate:\n");
        printf("  %04d-%02d-%02d\n",
               gps_data.utc_date.year,
               gps_data.utc_date.month,
               gps_data.utc_date.day);
    }

    /* Fix quality */
    printf("\nFix Quality:\n");
    printf("  Quality: %d (", gps_data.fix_quality);
    switch (gps_data.fix_quality) {
        case 0: printf("No fix"); break;
        case 1: printf("GPS fix"); break;
        case 2: printf("DGPS fix"); break;
        case 4: printf("RTK fix"); break;
        case 5: printf("RTK float"); break;
        default: printf("Unknown"); break;
    }
    printf(")\n");
    printf("  Satellites: %d\n", gps_data.satellites_used);

    if (gps_data.dop.valid) {
        printf("  HDOP: %.1f (", gps_data.dop.hdop);
        if (gps_data.dop.hdop < 1.0) printf("Excellent");
        else if (gps_data.dop.hdop < 2.0) printf("Good");
        else if (gps_data.dop.hdop < 5.0) printf("Moderate");
        else printf("Poor");
        printf(")\n");
    }

    /* Magnetic variation */
    if (gps_data.magnetic_variation_valid) {
        printf("\nMagnetic Variation:\n");
        printf("  %.1f° %c\n",
               gps_data.magnetic_variation,
               gps_data.magnetic_variation_direction);
    }

    /* Step 8: Cleanup */
    nmea_cleanup(&ctx);

    printf("\n=========================\n");
    printf("Example complete!\n");

    return 0;
}
