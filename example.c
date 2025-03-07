#include "clog.h"

CLOG_INIT();

int
main()
{
  clog_debug("This is a debug message\n");
  clog_info("This is an info message\n");
  clog_warn("This is a warning message\n");
  clog_error("This is an error message\n");

  FILE* fp = fopen("example.log", "w");
  clog_fdebug(fp, "This is a debug message\n");
  clog_finfo(fp, "This is an info message\n");
  clog_fwarn(fp, "This is a warning message\n");
  clog_ferror(fp, "This is an error message\n");
  fclose(fp);

  clog_guard_warn({
    clog_debug("[GUARD]: This is a debug message\n");
    clog_info("[GUARD]: This is an info message\n");
    clog_warn("[GUARD]: This is a warning message\n");
    clog_error("[GUARD]: This is an error message\n");
  });
  return 0;
}
