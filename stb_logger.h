// stb_logger.h (header-only library in stb-style)
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"

typedef enum {
    LOG_INFO,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

typedef struct {
    FILE* file;
    LogLevel level;
    int toConsole;
    pthread_mutex_t mutex;
} Logger;

const char* levelToString(LogLevel level);
Logger* createLogger(const char* filename, LogLevel level, int toConsole);
void logMessage(Logger* logger, LogLevel level, const char* format, ...);
void logHexMessage(Logger *logger, LogLevel level, const uint8_t *data, size_t length);
void closeLogger(Logger* logger);

#ifdef __cplusplus
}
#endif

#ifdef LOGGER_IMPLEMENTATION

const char* levelToString(LogLevel level) {
    switch (level) {
        case LOG_INFO: return "INFO";
        case LOG_ERROR: return "ERROR";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

Logger* createLogger(const char* filename, LogLevel level, int toConsole) {
    Logger* logger = (Logger*)malloc(sizeof(Logger));
    if (!logger) {
        perror("[ERROR] Failed to allocate memory for logger");
        return NULL;
    }
    logger->file = fopen(filename, "a");
    if (!logger->file) {
        perror("[ERROR] Failed to open log file");
        free(logger);
        return NULL;
    }
    logger->level = level;
    logger->toConsole = toConsole;
    pthread_mutex_init(&logger->mutex, NULL);
    return logger;
}

void logMessage(Logger* logger, LogLevel level, const char* format, ...) {
    if (level > logger->level) {
        return;
    }

    pthread_mutex_lock(&logger->mutex);

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    char header[64];
    snprintf(header, sizeof(header), "[%02d-%02d-%02d %02d:%02d:%02d] %s: ",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec, levelToString(level));

    va_list args;
    va_start(args, format);
    fprintf(logger->file, "%s", header);
    vfprintf(logger->file, format, args);
    fprintf(logger->file, "\n");
    fflush(logger->file);
    va_end(args);

    if (logger->toConsole) {
        const char* color = COLOR_RESET;
        if (level == LOG_INFO) {
            color = COLOR_GREEN;
        } else if (level == LOG_ERROR) {
            color = COLOR_RED;
        }

        va_start(args, format);
        fprintf((level == LOG_ERROR) ? stderr : stdout, "%s%s", color, header);
        vfprintf((level == LOG_ERROR) ? stderr : stdout, format, args);
        fprintf((level == LOG_ERROR) ? stderr : stdout, "%s\n", COLOR_RESET);
        va_end(args);
    }

    pthread_mutex_unlock(&logger->mutex);
}

void logHexMessage(Logger *logger, LogLevel level, const uint8_t *data, size_t length) {
    char* hexBuffer = (char*)malloc(6 * length + 1); // byte -> "' ' 0xXX + ' '"
    if (!hexBuffer) {
        perror("[ERROR] Failed to allocate memory for hex buffer");
        return;
    }
    memset(hexBuffer, 0, 6 * length + 1);
    size_t pos = 0;

    for (size_t i = 0; i < length; ++i) {
        pos += snprintf(hexBuffer + pos, 6, "0x%02X ", data[i]);
    }

    logMessage(logger, level, "Message of length %zu from client (hex): %s", length, hexBuffer);
    free(hexBuffer);
}

void closeLogger(Logger* logger) {
    if (logger) {
        fclose(logger->file);
        pthread_mutex_destroy(&logger->mutex);
        free(logger);
    }
}

#endif // LOGGER_IMPLEMENTATION

#endif // LOGGER_H
