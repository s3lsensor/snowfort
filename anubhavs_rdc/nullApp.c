#include <stdio.h>
#include "contiki.h"

PROCESS(null_app_process, "Null App Process");
AUTOSTART_PROCESSES(&null_app_process);

PROCESS_THREAD(null_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("Null App Started\n");
  PROCESS_END();
}
