# stb_logger

A lightweight and thread-safe logging library implemented in C. Designed as a single-header library in the style of stb libraries.

## Features
- Thread-safe logging with pthread mutexes.
- Logs messages with timestamps and levels (INFO, DEBUG, ERROR).
- Supports logging to a file and optionally to the console.
- Hexadecimal data logging for binary message debugging.
- Header-only implementation for simple integration.

## Getting Started

1. **Download the `stb_logger.h` file:**
   Clone the repository or copy `stb_logger.h` to your project directory.

2. **Include the header in your code:**
   ```c
   #include "stb_logger.h"
   ```
3. **Define the implementation macro in single source file:**
   ```c
   #define LOGGER_IMPLEMENTATION
   #include "stb_logger.h"
   ```
4. **Use the API:** 
   ```c
   #include "stb_logger.h"

    int main() {
      Logger* logger = createLogger("app.log", LOG_DEBUG, 1);

      if (!logger) {
          return 1; // Handle logger creation error
      }

      logMessage(logger, LOG_INFO, "Application started.");
      logMessage(logger, LOG_DEBUG, "Debugging value: %d", 42);

      uint8_t data[] = {0xAB, 0xCD, 0xEF};
      logHexMessage(logger, LOG_DEBUG, data, sizeof(data));

      logMessage(logger, LOG_ERROR, "An error occurred: %s", "example error");

      closeLogger(logger);
      return 0;
   }
   ```


## API Reference

### `Logger* createLogger(const char* filename, LogLevel level, int toConsole);`
Creates a new logger instance.
- `filename`: Path to the log file.
- `level`: Minimum log level (e.g., `LOG_INFO`).
- `toConsole`: Whether to also log to the console (1 for true, 0 for false).

### `void logMessage(Logger* logger, LogLevel level, const char* format, ...);`
Logs a formatted message with the specified log level.

### `void logHexMessage(Logger *logger, LogLevel level, const uint8_t *data, size_t length);`
Logs binary data in hexadecimal format.

### `void closeLogger(Logger* logger);`
Closes the logger and releases resources.

## Build and Requirements

- C99 or later.
- `pthread` library for threading support (POSIX systems).

## Contributing

Feel free to submit issues or pull requests to improve the library.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.


