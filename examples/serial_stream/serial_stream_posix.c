/**
 * @file serial_stream_posix.c
 * @brief Example of parsing NMEA data from a serial port on POSIX systems
 *
 * This example demonstrates:
 * 1. Opening and configuring a serial port (UART)
 * 2. Buffering incoming data byte-by-byte
 * 3. Parsing complete NMEA sentences
 * 4. Handling errors and incomplete data
 *
 * Platform: Linux, macOS, *BSD, or any POSIX-compliant system
 * Hardware: GPS module connected via USB-to-serial adapter or UART
 *
 * Usage:
 *   ./serial_stream_posix /dev/ttyUSB0 9600
 *   ./serial_stream_posix /dev/cu.usbserial 4800
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "nmea.h"

/* Configuration */
#define LINE_BUFFER_SIZE 128
#define PARSE_BUFFER_SIZE 256

/* Global state for signal handling */
static volatile bool keep_running = true;

/* Signal handler for clean shutdown */
void signal_handler(int signum) {
  (void)signum;
  keep_running = false;
  printf("\nShutting down...\n");
}

/**
 * Setup serial port with given baud rate
 */
int setup_serial_port(const char* device, int baudrate) {
  int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror("Failed to open serial port");
    return -1;
  }

  /* Configure serial port */
  struct termios options;
  memset(&options, 0, sizeof(options));

  if (tcgetattr(fd, &options) != 0) {
    perror("Failed to get serial port attributes");
    close(fd);
    return -1;
  }

  /* Set baud rate */
  speed_t speed;
  switch (baudrate) {
    case 4800:
      speed = B4800;
      break;
    case 9600:
      speed = B9600;
      break;
    case 19200:
      speed = B19200;
      break;
    case 38400:
      speed = B38400;
      break;
    case 57600:
      speed = B57600;
      break;
    case 115200:
      speed = B115200;
      break;
    default:
      fprintf(stderr, "Unsupported baud rate: %d\n", baudrate);
      close(fd);
      return -1;
  }

  cfsetispeed(&options, speed);
  cfsetospeed(&options, speed);

  /* 8N1 configuration */
  options.c_cflag &= ~PARENB; /* No parity */
  options.c_cflag &= ~CSTOPB; /* 1 stop bit */
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8; /* 8 data bits */

  /* Enable receiver, ignore modem control lines */
  options.c_cflag |= (CLOCAL | CREAD);

  /* Raw input mode */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  /* Raw output mode */
  options.c_oflag &= ~OPOST;

  /* No flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY);

  /* No special character handling */
  options.c_iflag &=
      ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

  /* Non-blocking reads with timeout */
  options.c_cc[VMIN] = 0;  /* Non-blocking */
  options.c_cc[VTIME] = 1; /* 0.1 second timeout */

  /* Apply settings */
  if (tcsetattr(fd, TCSANOW, &options) != 0) {
    perror("Failed to set serial port attributes");
    close(fd);
    return -1;
  }

  /* Flush buffers */
  tcflush(fd, TCIOFLUSH);

  return fd;
}

/**
 * Line buffer for accumulating NMEA sentences
 */
typedef struct {
  char buffer[LINE_BUFFER_SIZE];
  size_t pos;
  uint32_t overflow_count;
} line_buffer_t;

void line_buffer_init(line_buffer_t* lb) {
  lb->pos = 0;
  lb->overflow_count = 0;
}

bool line_buffer_add_byte(line_buffer_t* lb, char byte) {
  /* Check for overflow */
  if (lb->pos >= sizeof(lb->buffer) - 1) {
    lb->overflow_count++;
    lb->pos = 0;
    fprintf(stderr, "Warning: Line buffer overflow (count: %u)\n",
            lb->overflow_count);
    return false;
  }

  /* Add byte to buffer */
  lb->buffer[lb->pos++] = byte;

  /* Check for complete sentence */
  if (byte == '\n') {
    lb->buffer[lb->pos] = '\0';
    return true;
  }

  return false;
}

/**
 * Statistics tracking
 */
typedef struct {
  uint32_t total_bytes;
  uint32_t total_sentences;
  uint32_t successful_parses;
  uint32_t checksum_errors;
  uint32_t invalid_sentences;
  uint32_t unknown_sentences;
} parse_stats_t;

void print_stats(parse_stats_t* stats) {
  printf("\n=== Statistics ===\n");
  printf("Total bytes:      %u\n", stats->total_bytes);
  printf("Total sentences:  %u\n", stats->total_sentences);
  printf("Successful:       %u (%.1f%%)\n", stats->successful_parses,
         stats->total_sentences > 0
             ? (float)stats->successful_parses / stats->total_sentences * 100.0f
             : 0.0f);
  printf("Checksum errors:  %u\n", stats->checksum_errors);
  printf("Invalid:          %u\n", stats->invalid_sentences);
  printf("Unknown/Disabled: %u\n", stats->unknown_sentences);
}

/**
 * Display GPS data
 */
void display_gps_data(nmea_gnss_state_t* gnss) {
  printf("\n--- GPS Data ---\n");

  if (gnss->latitude.valid && gnss->longitude.valid) {
    printf("Position: %.6f° %c, %.6f° %c\n", fabs(gnss->latitude.degrees),
           gnss->latitude.degrees >= 0 ? 'N' : 'S',
           fabs(gnss->longitude.degrees),
           gnss->longitude.degrees >= 0 ? 'E' : 'W');
  } else {
    printf("Position: No fix\n");
  }

  if (gnss->altitude_valid) {
    printf("Altitude: %.1f m\n", gnss->altitude_meters);
  }

  if (gnss->utc_time.valid) {
    printf("Time: %02d:%02d:%02d UTC\n", gnss->utc_time.hour,
           gnss->utc_time.minute, gnss->utc_time.second);
  }

  if (gnss->speed.valid) {
    printf("Speed: %.1f knots (%.1f km/h)\n", gnss->speed.knots,
           gnss->speed.kmh);
  }

  printf("Fix quality: %d\n", gnss->fix_quality);
  printf("Satellites: %d\n", gnss->satellites_used);

  if (gnss->dop.valid) {
    printf("HDOP: %.1f\n", gnss->dop.hdop);
  }
}

/**
 * Main program
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <device> <baudrate>\n", argv[0]);
    fprintf(stderr, "Example: %s /dev/ttyUSB0 9600\n", argv[0]);
    return 1;
  }

  const char* device = argv[1];
  int baudrate = atoi(argv[2]);

  /* Setup signal handler */
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  /* Open serial port */
  printf("Opening serial port: %s at %d baud\n", device, baudrate);
  int serial_fd = setup_serial_port(device, baudrate);
  if (serial_fd < 0) {
    return 1;
  }

  printf("Serial port opened successfully\n");

  /* Initialize NMEA parser */
  nmea_context_t ctx;
  nmea_config_t config = {
      .enabled_modules = (1 << NMEA_MODULE_GNSS) | (1 << NMEA_MODULE_SENSOR),
      .validate_checksums = true,
      .error_callback = NULL};

  if (nmea_init(&ctx, &config) != NMEA_OK) {
    fprintf(stderr, "Failed to initialize NMEA parser\n");
    close(serial_fd);
    return 1;
  }

  printf("NMEA parser initialized\n");
  printf("Waiting for data... (Press Ctrl+C to exit)\n\n");

  /* Buffers */
  line_buffer_t line_buf;
  line_buffer_init(&line_buf);

  char parse_buffer[PARSE_BUFFER_SIZE];
  parse_stats_t stats = {0};

  uint32_t display_counter = 0;
  nmea_gnss_state_t last_gnss = {0};

  /* Main loop */
  while (keep_running) {
    char byte;
    ssize_t n = read(serial_fd, &byte, 1);

    if (n > 0) {
      stats.total_bytes++;

      /* Add byte to line buffer */
      if (line_buffer_add_byte(&line_buf, byte)) {
        /* Complete sentence received */
        stats.total_sentences++;

        /* Only parse if it starts with $ or ! */
        if (line_buf.buffer[0] == '$' || line_buf.buffer[0] == '!') {
          /* Parse sentence */
          nmea_result_t result = nmea_parse(&ctx, line_buf.buffer, line_buf.pos,
                                            parse_buffer, sizeof(parse_buffer));

          /* Handle result */
          switch (result) {
            case NMEA_OK:
              stats.successful_parses++;

              /* Get GPS data */
              if (nmea_get_gnss_data(&ctx, &last_gnss) == NMEA_OK) {
                /* Display every 10th successful parse */
                if (++display_counter >= 10) {
                  display_gps_data(&last_gnss);
                  display_counter = 0;
                }
              }
              break;

            case NMEA_ERROR_CHECKSUM_FAILED:
              stats.checksum_errors++;
              fprintf(stderr, "Checksum error in: %s", line_buf.buffer);
              break;

            case NMEA_ERROR_INVALID_SENTENCE:
              stats.invalid_sentences++;
              break;

            case NMEA_ERROR_UNKNOWN_SENTENCE:
            case NMEA_ERROR_SENTENCE_DISABLED:
              stats.unknown_sentences++;
              break;

            default:
              fprintf(stderr, "Parse error: %s\n",
                      nmea_get_error_string(result));
              break;
          }
        }

        /* Reset line buffer */
        line_buffer_init(&line_buf);
      }
    } else if (n < 0 && errno != EAGAIN) {
      perror("Read error");
      break;
    }
  }

  /* Cleanup */
  print_stats(&stats);
  nmea_cleanup(&ctx);
  close(serial_fd);

  printf("\nProgram terminated\n");
  return 0;
}
