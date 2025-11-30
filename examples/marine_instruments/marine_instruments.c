/**
 * @file marine_instruments.c
 * @brief Marine/legacy instrument integration example
 *
 * This example shows how to parse data from traditional marine instruments
 * and sensors that output NMEA-0183 data:
 * - Depth sounders (DBT, DPT)
 * - Water temperature sensors (MTW)
 * - Wind instruments (MWD, MWV)
 * - Compass/heading sensors (HDT, HDG, HDM)
 * - Speed through water (VHW, VBW)
 * - Rate of turn (ROT)
 *
 * Perfect for: Sailboats, motor yachts, older marine electronics integration
 */

#include <stdio.h>
#include <string.h>
#include "nmea.h"

/* Display functions for different instrument types */

void display_depth(nmea_sensor_state_t *sensor) {
    if (!sensor->depth_valid) {
        return;
    }

    printf("\n┌─ DEPTH SOUNDER ─────────────────┐\n");
    printf("│ Depth:    %6.1f m (%5.1f ft)   │\n",
           sensor->depth_below_transducer_meters,
           sensor->depth_below_transducer_feet);
    printf("│           %6.1f fathoms        │\n",
           sensor->depth_below_transducer_fathoms);

    if (sensor->depth_offset_meters != 0) {
        printf("│ Offset:   %6.1f m             │\n",
               sensor->depth_offset_meters);
    }

    if (sensor->depth_range_meters > 0) {
        printf("│ Max depth:%6.1f m             │\n",
               sensor->depth_range_meters);
    }

    printf("└─────────────────────────────────┘\n");
}

void display_water_temp(nmea_sensor_state_t *sensor) {
    if (!sensor->water_temperature_valid) {
        return;
    }

    printf("\n┌─ WATER TEMPERATURE ─────────────┐\n");
    printf("│ Temperature: %5.1f°C            │\n",
           sensor->water_temperature_celsius);
    printf("│              %5.1f°F            │\n",
           sensor->water_temperature_celsius * 9.0f / 5.0f + 32.0f);
    printf("└─────────────────────────────────┘\n");
}

void display_wind(nmea_sensor_state_t *sensor) {
    if (!sensor->wind_valid) {
        return;
    }

    printf("\n┌─ WIND INSTRUMENTS ──────────────┐\n");

    if (sensor->wind_direction_true > 0) {
        printf("│ Direction:   %5.1f° True       │\n",
               sensor->wind_direction_true);
    }

    if (sensor->wind_direction_magnetic > 0) {
        printf("│              %5.1f° Magnetic   │\n",
               sensor->wind_direction_magnetic);
    }

    printf("│ Speed:       %5.1f knots       │\n", sensor->wind_speed_knots);
    printf("│              %5.1f m/s         │\n", sensor->wind_speed_mps);
    printf("│              %5.1f km/h        │\n", sensor->wind_speed_kmh);

    if (sensor->wind_reference == 'R') {
        printf("│ Reference:   Relative          │\n");
    } else if (sensor->wind_reference == 'T') {
        printf("│ Reference:   True              │\n");
    }

    printf("└─────────────────────────────────┘\n");
}

void display_heading(nmea_heading_state_t *heading) {
    bool has_data = false;

    printf("\n┌─ HEADING/COMPASS ───────────────┐\n");

    if (heading->hdt_valid) {
        printf("│ True heading:      %6.1f°    │\n", heading->heading_true);
        has_data = true;
    }

    if (heading->hdg_valid) {
        printf("│ Magnetic heading:  %6.1f°    │\n", heading->heading_magnetic_sensor);
        has_data = true;
    }

    if (heading->hdg_valid && heading->magnetic_variation != 0) {
        printf("│ Variation:         %6.1f° %c  │\n",
               heading->magnetic_variation,
               heading->variation_direction);
        has_data = true;
    }

    if (heading->hdg_valid && heading->magnetic_deviation != 0) {
        printf("│ Deviation:         %6.1f° %c  │\n",
               heading->magnetic_deviation,
               heading->deviation_direction);
        has_data = true;
    }

    if (!has_data) {
        printf("│ No heading data                │\n");
    }

    printf("└─────────────────────────────────┘\n");
}

void display_rate_of_turn(nmea_heading_state_t *heading) {
    if (!heading->rot_valid) {
        return;
    }

    printf("\n┌─ RATE OF TURN ──────────────────┐\n");
    printf("│ ROT:  %+6.1f°/min             │\n", heading->rate_of_turn);

    if (heading->rate_of_turn > 0) {
        printf("│       Turning right             │\n");
    } else if (heading->rate_of_turn < 0) {
        printf("│       Turning left              │\n");
    } else {
        printf("│       Steady course             │\n");
    }

    printf("└─────────────────────────────────┘\n");
}

void display_speed_through_water(nmea_sensor_state_t *sensor) {
    if (!sensor->water_speed_heading_valid && !sensor->speed_valid) {
        return;
    }

    printf("\n┌─ SPEED THROUGH WATER ───────────┐\n");

    if (sensor->water_speed_knots > 0) {
        printf("│ Speed:  %5.1f knots            │\n", sensor->water_speed_knots);
        printf("│         %5.1f km/h             │\n",
               sensor->water_speed_knots * 1.852f);
    } else if (sensor->speed_water_knots > 0) {
        printf("│ Speed:  %5.1f knots            │\n", sensor->speed_water_knots);
    }

    if (sensor->speed_ground_knots > 0) {
        printf("│ SOG:    %5.1f knots (GPS)      │\n", sensor->speed_ground_knots);
    }

    printf("└─────────────────────────────────┘\n");
}

void display_distance_log(nmea_sensor_state_t *sensor) {
    if (sensor->distance_water_nm <= 0 && sensor->distance_ground_nm <= 0) {
        return;
    }

    printf("\n┌─ DISTANCE LOG ──────────────────┐\n");

    if (sensor->distance_water_nm > 0) {
        printf("│ Through water: %8.1f nm     │\n", sensor->distance_water_nm);
    }

    if (sensor->distance_ground_nm > 0) {
        printf("│ Over ground:   %8.1f nm     │\n", sensor->distance_ground_nm);
    }

    printf("└─────────────────────────────────┘\n");
}

int main(void) {
    printf("╔═══════════════════════════════════════╗\n");
    printf("║  Marine Instruments Example           ║\n");
    printf("║  Legacy NMEA-0183 Sensor Integration  ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    /* Initialize parser with sensor and heading modules */
    nmea_context_t ctx;

    nmea_config_t config = {
        .enabled_modules = (1 << NMEA_MODULE_SENSOR) |   /* Depth, wind, temp */
                          (1 << NMEA_MODULE_HEADING),    /* Compass, ROT */
        .validate_checksums = false,  /* Legacy equipment may not have checksums */
        .error_callback = NULL
    };

    if (nmea_init(&ctx, &config) != NMEA_OK) {
        fprintf(stderr, "Failed to initialize parser\n");
        return 1;
    }

    printf("\nParser initialized (Sensor + Heading modules)\n");
    printf("Memory usage: %zu bytes\n", nmea_get_context_size());

    /* Buffer for parsing */
    char parse_buffer[256];

    /* Simulated data from various marine instruments */
    const char *instrument_data[] = {
        /* Depth sounder - 12.3 feet / 3.7 meters / 2.0 fathoms */
        "$SDDBT,12.3,f,3.7,M,2.0,F*3A\r\n",

        /* Water temperature - 15.5°C */
        "$IIMTW,15.5,C*21\r\n",

        /* Wind direction and speed */
        "$IIMWD,045.0,T,048.5,M,15.2,N,7.8,M*52\r\n",

        /* Wind speed and angle (relative) */
        "$IIMWV,045.0,R,15.2,N,A*25\r\n",

        /* True heading */
        "$HCHDT,234.5,T*1C\r\n",

        /* Magnetic heading with variation */
        "$HCHDG,231.0,,,3.5,E*39\r\n",

        /* Rate of turn - turning right at 2.5°/min */
        "$HEROT,2.5,A*3F\r\n",

        /* Water speed and heading */
        "$IIVHW,234.5,T,231.0,M,6.5,N,12.0,K*52\r\n",

        /* Speed through water and over ground */
        "$IIVBW,6.5,0.0,A,7.2,0.0,A,0.0,A,0.0,A*4E\r\n",

        /* Distance log */
        "$IIVLW,1234.5,N,123.4,N,987.6,N,98.7,N*43\r\n",

        /* Depth with offset (DPT) */
        "$SDDPT,3.7,0.5,100.0*47\r\n",

        NULL
    };

    /* Parse all sentences */
    printf("\n");
    printf("═══════════════════════════════════════\n");
    printf(" Parsing instrument data...\n");
    printf("═══════════════════════════════════════\n");

    int parsed_count = 0;
    for (int i = 0; instrument_data[i] != NULL; i++) {
        nmea_result_t result = nmea_parse(
            &ctx,
            instrument_data[i],
            strlen(instrument_data[i]),
            parse_buffer,
            sizeof(parse_buffer)
        );

        if (result == NMEA_OK) {
            parsed_count++;
            /* Extract sentence type for display */
            char sentence_type[4] = {0};
            if (strlen(instrument_data[i]) > 6) {
                memcpy(sentence_type, instrument_data[i] + 3, 3);
            }
            printf("  [OK] %s parsed\n", sentence_type);
        }
    }

    printf("\nParsed %d sentences successfully\n", parsed_count);

    /* Retrieve and display all instrument data */
    nmea_sensor_state_t sensor_data;
    nmea_heading_state_t heading_data;

    printf("\n");
    printf("═══════════════════════════════════════\n");
    printf(" Instrument Readings\n");
    printf("═══════════════════════════════════════\n");

    if (nmea_get_sensor_data(&ctx, &sensor_data) == NMEA_OK) {
        display_depth(&sensor_data);
        display_water_temp(&sensor_data);
        display_wind(&sensor_data);
        display_speed_through_water(&sensor_data);
        display_distance_log(&sensor_data);
    }

    if (nmea_get_heading_data(&ctx, &heading_data) == NMEA_OK) {
        display_heading(&heading_data);
        display_rate_of_turn(&heading_data);
    }

    /* Summary for navigation */
    printf("\n");
    printf("═══════════════════════════════════════\n");
    printf(" Navigation Summary\n");
    printf("═══════════════════════════════════════\n");

    if (heading_data.hdt_valid && sensor_data.water_speed_knots > 0) {
        printf("\n Current course: %.1f° True\n", heading_data.heading_true);
        printf(" Speed:          %.1f knots\n", sensor_data.water_speed_knots);

        if (sensor_data.depth_valid) {
            printf(" Depth:          %.1f m\n", sensor_data.depth_below_transducer_meters);
        }

        if (sensor_data.wind_valid) {
            printf(" Wind:           %.0f° at %.1f knots\n",
                   sensor_data.wind_direction_true,
                   sensor_data.wind_speed_knots);
        }
    }

    /* Cleanup */
    nmea_cleanup(&ctx);

    printf("\n");
    printf("═══════════════════════════════════════\n");
    printf(" Example complete!\n");
    printf("═══════════════════════════════════════\n");

    return 0;
}
