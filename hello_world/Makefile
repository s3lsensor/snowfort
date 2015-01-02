CONTIKI=$(PWD)/..
APPS = serial-shell shell remote-shell
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CFLAGS += $(BASH_CONST)
CFLAGS += -DUART1_CONF_TX_WITH_INTERRUPT=1
CONTIKI_SOURCEFILES += app_util.c 
include $(CONTIKI)/Makefile.include

sensor_nocomm.co: In function `process_thread_null_app_process':
sensor_nocomm.c:(.text.process_thread_null_app_process+0x2a): undefined reference to `tsl2561_poweron'
sensor_nocomm.c:(.text.process_thread_null_app_process+0x64): undefined reference to `tsl2561_sample'
collect2: ld returned 1 exit status
make: *** [sensor_nocomm.sky] Error 1

