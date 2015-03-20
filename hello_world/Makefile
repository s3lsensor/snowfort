CONTIKI=$(PWD)/..
APPS = serial-shell shell remote-shell
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CFLAGS += $(BASH_CONST)
CFLAGS += -DUART1_CONF_TX_WITH_INTERRUPT=1
CONTIKI_SOURCEFILES += app_util.c
HTU21D = 1
include Makefile.target
ifeq ($(TARGET), sky)

ifdef MPU_6050
CONTIKI_SOURCEFILES += mpu-6050.c
endif

ifdef HTU21D
CONTIKI_SOURCEFILES += htu21d.c
endif

ifdef 6DOF
CONTIKI_SOURCEFILES += 6dof.c
endif

ifdef ISL29125
CONTIKI_SOURCEFILES += isl29125.c
endif

ifdef ML8511
CONTIKI_SOURCEFILES += ml8511.c
endif

ifdef MS5803
CONTIKI_SOURCEFILES += ms5803.c
endif

ifdef SOUNDDET
CONTIKI_SOURCEFILES += soundDet.c
endif

ifdef TSL2561
CONTIKI_SOURCEFILES += tsl2561.c
endif

endif
include $(CONTIKI)/Makefile.include

