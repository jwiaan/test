.PHONY: run gdb initrd
q := qemu-system-x86_64 -kernel ../linux/arch/x86/boot/bzImage -initrd initrd.cpio.gz -nographic -append 'nokaslr console=ttyS0'
run:
	$(q)
gdb:
	$(q) -s -S
initrd:
	cd initrd && find | cpio -oH newc | gzip > ../initrd.cpio.gz
