CFILES = pmbus-core.c

HFILES = pmbus-core.h pmbus-ctrl.h

DRIVER_NAME=caswell-pmbus

obj-m += $(DRIVER_NAME).o

$(DRIVER_NAME)-objs := $(CFILES:.c=.o)

KSRC  ?= /lib/modules/$(shell uname -r)/build

all: clean
	$(MAKE) -C $(KSRC) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KSRC) M=$(shell pwd) clean
	@rm -rf Module.markers Module.symvers modules.order

