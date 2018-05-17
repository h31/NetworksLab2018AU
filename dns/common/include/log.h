#ifndef AUDNS_LOG_H
#define AUDNS_LOG_H

#ifdef LOG_ENABLE
  #define LOG(...) do { fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while (0)
#else
  #define LOG(...) ((void) 0)
#endif

#endif  // AUDNS_LOG_H
