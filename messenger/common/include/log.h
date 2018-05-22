#ifndef ELEGRAM_LOG_H
#define ELEGRAM_LOG_H

#define LOG(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while (0)

#endif  // ELEGRAM_LOG_H
