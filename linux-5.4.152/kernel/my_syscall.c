#include <linux/kernel.h>

asmlinkage long sys_mycall_(void) {
	printk("System Call Example!\n");

	return 0;
}
