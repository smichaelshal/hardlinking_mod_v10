ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m := hardlinkingmod.o
hardlinkingmod-y := hardlinking_mod.o
hardlinkingmod-y += hardlinking/hardlinking.o hardlinking/namei_utils.o

ccflags-y := -I$(src)
ccflags-y += -I$(src)/hardlinking


else
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.mod modules.order *.symvers .*.o.d .*.o.cmd
endif

# '*.o' -o -name '*~' -o -name 'core' -o -name '.depend' -o -name '.*.cmd' -o -name '*.ko' -o -name '*.mod.c' -o -name '.tmp_versions' -o -name '*.mod' -o -name 'modules.order' -o -name '*.symvers' -o -name '.*.o.d' -o -name '.*.o.cmd'
# find . -type f -name '.*.o.d' -o -name '.*.o.cmd'  -o -name '' -exec rm -rf {} +
# 


# find . -type f \
# 	-not -name "*.c" \
# 	-not -name "*.h" \
# 	-not -name "README" \
# 	-not -name "Makefile" \
# 	-not -name "Kbuild" \
# 	-exec rm -rf {} +