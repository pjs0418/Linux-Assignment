#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kthread.h>

int thread_1(void *data) {
	int *value = (int *)data;
	printk(KERN_INFO "Thread 1 initial value : %d\n", *value);

	__sync_fetch_and_add(value, 1);
	printk(KERN_INFO "Thread 1 value after fetch-and-add : %d\n", *value);

	__sync_lock_test_and_set(value, 5);
	printk(KERN_INFO "Thread 1 value after test-and-set : %d\n", *value);

	__sync_val_compare_and_swap(value, 5, 3);
	printk(KERN_INFO "Thread 1 value after compare-and-swap : %d\n", *value);

	return 0;
}


int thread_2(void *data) {
	int *value = (int *)data;
	printk(KERN_INFO "Thread 2 initial value : %d\n", *value);

	__sync_fetch_and_add(value, 2);
	printk(KERN_INFO "Thread 2 value after fetch-and-add : %d\n", *value);

	__sync_lock_test_and_set(value, 10);
	printk(KERN_INFO "Thread 2 value after test-and-set : %d\n", *value);

	__sync_val_compare_and_swap(value, 10, 5);
	printk(KERN_INFO "Thread 2 value after compare-and-swap : %d\n", *value);

	return 0;
}

int thread_3(void *data) {
	int *value = (int *)data;
	printk(KERN_INFO "Thread 3 initial value : %d\n", *value);

	__sync_fetch_and_add(value, 3);
	printk(KERN_INFO "Thread 3 value after fetch-and-add : %d\n", *value);

	__sync_lock_test_and_set(value, 15);
	printk(KERN_INFO "Thread 3 value after test-and-set : %d\n", *value);

	__sync_val_compare_and_swap(value, 15, 8);
	printk(KERN_INFO "Thread 3 value after compare-and-swap : %d\n", *value);

	return 0;
}

int thread_4(void *data) {
	int *value = (int *)data;
	printk(KERN_INFO "Thread 4 initial value : %d\n", *value);

	__sync_fetch_and_add(value, 4);
	printk(KERN_INFO "Thread 4 value after fetch-and-add : %d\n", *value);

	__sync_lock_test_and_set(value, 20);
	printk(KERN_INFO "Thread 4 value after test-and-set : %d\n", *value);

	__sync_val_compare_and_swap(value, 20, 10);
	printk(KERN_INFO "Thread 4 value after compare-and-swap : %d\n", *value);

	return 0;
}

int __init test_module_init(void) {
	int *data = (int *)kmalloc(sizeof(int), GFP_KERNEL);
	*data = 0;

	kthread_run(&thread_1, (void *)data, "thread 1");
	kthread_run(&thread_2, (void *)data, "thread 2");
	kthread_run(&thread_3, (void *)data, "thread 3");
	kthread_run(&thread_4, (void *)data, "thread 4");

	return 0;
}

void __exit test_module_cleanup(void) {
	printk("Bye Module\n");
}

module_init(test_module_init);
module_exit(test_module_cleanup);

