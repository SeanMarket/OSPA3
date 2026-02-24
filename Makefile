obj-m := char_driver.o
all:
	make  -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)


clean:
	find ./ -type f -not -name 'hello_module.c' -not -name 'Makefile' -not -name 'char_driver.c' -not -name 'sample_char_driver' -delete
