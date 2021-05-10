# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)

	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default: module query_app test

module:
	$(MAKE) -C $(KERNEL_SOURCE) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) M=$(shell pwd) clean
	${RM} query_app
	${RM} test

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else

	obj-m := query_ioctl.o

endif
