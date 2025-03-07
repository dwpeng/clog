#ifndef __clog_h__
#define __clog_h__

#define cplatform_unknown 0
#define cplatform_windows 1
#define cplatform_linux 2
#define cplatform_macos 3

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#define cplatform cplatform_windows
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#define cplatform cplatform_linux
#elif defined(__APPLE__) || defined(__MACH__)
#define cplatform cplatform_macos
#else
#define cplatform 0
#warning "Unknown platform"
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#if cplatform == cplatform_linux || cplatform == cplatform_macos
#include <unistd.h>
#define ISATTY(fp) isatty(fileno(fp))
#elif cplatform == cplatform_windows
#include <io.h>
#define ISATTY(fp) _isatty(_fileno(fp))
#else
#define ISATTY(fp) 0
#endif

#if defined(CLOG_NCOLOR)
#undef ISATTY
#define ISATTY(fp) 0
#endif

typedef enum {
  CLOG_LEVEL_DEBUG = 10,
  CLOG_LEVEL_INFO = 20,
  CLOG_LEVEL_WARN = 30,
  CLOG_LEVEL_ERROR = 40,
} clog_level_t;

static const char* __PLAIN_LEVELS[] = {
  "DEBUG",
  "INFO ",
  "WARN ",
  "ERROR",
};
static const char* __COLOR_LEVELS[] = {
  "\033[32mDEBUG\033[0m",
  "\033[34mINFO \033[0m",
  "\033[33mWARN \033[0m",
  "\033[31mERROR\033[0m",
};

#define __levelStr(level)                                                                                              \
  (level == CLOG_LEVEL_DEBUG   ? __PLAIN_LEVELS[0]                                                                     \
   : level == CLOG_LEVEL_INFO  ? __PLAIN_LEVELS[1]                                                                     \
   : level == CLOG_LEVEL_WARN  ? __PLAIN_LEVELS[2]                                                                     \
   : level == CLOG_LEVEL_ERROR ? __PLAIN_LEVELS[3]                                                                     \
                               : "UNKNOWN")

#define __levelColorStr(level)                                                                                         \
  (level == CLOG_LEVEL_DEBUG   ? __COLOR_LEVELS[0]                                                                     \
   : level == CLOG_LEVEL_INFO  ? __COLOR_LEVELS[1]                                                                     \
   : level == CLOG_LEVEL_WARN  ? __COLOR_LEVELS[2]                                                                     \
   : level == CLOG_LEVEL_ERROR ? __COLOR_LEVELS[3]                                                                     \
                               : "UNKNOWN")

#define _CLOG_STR(x) #x
#define CLOG_StR(x) _CLOG_STR(x)

#ifndef NDEBUG
#define __header(file, line) "[" file ":" line "]"
#define __headerColor(file, line) "[\033[36m" file ":" line "\033[0m]"
#else
#define __header(file, line) ""
#define __headerColor(file, line) ""
#endif

static clog_level_t __clog_default_level = CLOG_LEVEL_DEBUG;

static inline void
clog_set_level(clog_level_t level)
{
  __clog_default_level = level;
}

static inline clog_level_t
clog_get_level()
{
  return __clog_default_level;
}

static inline FILE*
__pglog_print_file(int level, FILE* stream, const char* header)
{
  if (level < __clog_default_level)
    return NULL;
  char buff[30] = { 0 };
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", t);
  fprintf(stream, "[%s - %s]%s:", __levelStr(level), buff, header);
  return stream;
}

static inline FILE*
__pglog_print(int level, FILE* stream, const char* header)
{
  if (level < __clog_default_level)
    return NULL;
  char buff[30] = { 0 };
  time_t now = time(NULL);
  struct tm* t = localtime(&now);
  strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", t);
  fprintf(stream, "[%s - \033[36m%s\033[0m]%s:", __levelColorStr(level), buff, header);
  return stream;
}

#define __log2file(level, fp, ...)                                                                                     \
  do {                                                                                                                 \
    FILE* __fp = __pglog_print_file(level, fp, __header(__FILE__, CLOG_StR(__LINE__)));                                \
    if (__fp) {                                                                                                        \
      fprintf(__fp, __VA_ARGS__);                                                                                      \
      fflush(__fp);                                                                                                    \
    }                                                                                                                  \
  } while (0)

#define __log2terminal(level, ...)                                                                                     \
  do {                                                                                                                 \
    FILE* __tfp = level == CLOG_LEVEL_ERROR ? stderr : stdout;                                                         \
    if (!ISATTY(__tfp)) {                                                                                              \
      __log2file(level, __tfp, __VA_ARGS__);                                                                           \
    } else {                                                                                                           \
      FILE* __fp = __pglog_print(level, __tfp, __headerColor(__FILE__, CLOG_StR(__LINE__)));                           \
      if (__fp) {                                                                                                      \
        fprintf(__fp, __VA_ARGS__);                                                                                    \
        fflush(__tfp);                                                                                                 \
      }                                                                                                                \
    }                                                                                                                  \
  } while (0)

#ifndef CLOG_DISABLE
#define clog_debug(...) __log2terminal(CLOG_LEVEL_DEBUG, __VA_ARGS__)
#define clog_info(...) __log2terminal(CLOG_LEVEL_INFO, __VA_ARGS__)
#define clog_warn(...) __log2terminal(CLOG_LEVEL_WARN, __VA_ARGS__)
#define clog_error(...) __log2terminal(CLOG_LEVEL_ERROR, __VA_ARGS__)

#define clog_fdebug(stream, ...) __log2file(CLOG_LEVEL_DEBUG, stream, __VA_ARGS__)
#define clog_finfo(stream, ...) __log2file(CLOG_LEVEL_INFO, stream, __VA_ARGS__)
#define clog_fwarn(stream, ...) __log2file(CLOG_LEVEL_WARN, stream, __VA_ARGS__)
#define clog_ferror(stream, ...) __log2file(CLOG_LEVEL_ERROR, stream, __VA_ARGS__)
#else
#define clog_unused(x) ((void)(x))
#define clog_debug(...)
#define clog_info(...)
#define clog_warn(...)
#define clog_error(...)
#define clog_fdebug(stream, ...) clog_unused(stream)
#define clog_finfo(stream, ...) clog_unused(stream)
#define clog_fwarn(stream, ...) clog_unused(stream)
#define clog_ferror(stream, ...) clog_unused(stream)
#endif

#define clog_guard(guard_level, expr)                                                                                  \
  do {                                                                                                                 \
    clog_level_t __old_level = __clog_default_level;                                                                   \
    __clog_default_level = guard_level;                                                                                \
    expr;                                                                                                              \
    __clog_default_level = __old_level;                                                                                \
  } while (0)

#define clog_guard_debug(expr) clog_guard(CLOG_LEVEL_DEBUG, expr)
#define clog_guard_info(expr) clog_guard(CLOG_LEVEL_INFO, expr)
#define clog_guard_warn(expr) clog_guard(CLOG_LEVEL_WARN, expr)
#define clog_guard_error(expr) clog_guard(CLOG_LEVEL_ERROR, expr)

#endif // __clog_h__
