.PHONY: test

namem = sys_mod
namef = /sys/ant_sys_dir/ant_sys_file

obj-m += $(namem).o

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

rm:
		rmmod -f $(namem).ko

cli:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

clean: rm cli

test:
		insmod $(namem).ko
		echo rofl > $(namef)
		echo kekichya > $(namef)
		cat $(namef)
		echo end > $(namef)
		cat $(namef)
