CONTIKI=$(PWD)/..
APPS = serial-shell shell remote-shell
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CFLAGS += $(BASH_CONST)
CFLAGS += -DUART1_CONF_TX_WITH_INTERRUPT=1
CONTIKI_SOURCEFILES += app_util.c 
include $(CONTIKI)/Makefile.include

