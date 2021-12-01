#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/ktime.h>
#include <linux/kthread.h>

#define BILLION 1000000000

void spinlock_example(int num);
int thread_1(void *num);
int thread_2(void *num);
int thread_3(void *num);
int thread_4(void *num);

struct my_node {
	struct list_head entry;
	int data;
};

static struct list_head my_list;
static DEFINE_SPINLOCK(counter_lock);

int __init spinlock_module_init(void) {
	spinlock_example(25000);
	return 0;
}

void __exit spinlock_module_cleanup(void) {
	printk("Bye Module!\n");
}

module_init(spinlock_module_init);
module_exit(spinlock_module_cleanup);
MODULE_LICENSE("GPL");

void spinlock_example(int num) {
	INIT_LIST_HEAD(&my_list);

	int *count = (int *)kmalloc(sizeof(int), GFP_KERNEL);
	*count = num;

	kthread_run(&thread_1, (void *)count, "thread 1");
	kthread_run(&thread_2, (void *)count, "thread 2");
	kthread_run(&thread_3, (void *)count, "thread 3");
	kthread_run(&thread_4, (void *)count, "thread 4");
}

int thread_1(void *num) {
	int *count = (int*)num;
	int number = *count;
	int i, search_data;
	ktime_t start_time, end_time, total_time;
	struct my_node *current_node = NULL, *tmp;

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_add(&new->entry, &my_list);
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list insert time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(search_data = 0;search_data < number;search_data++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry(current_node, &my_list, entry) {
			if(current_node->data == search_data) {
				break;
			}
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list search time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry_safe(current_node, tmp, &my_list, entry) {
			list_del(&current_node->entry);
			kfree(current_node);
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list delete time : %llu ns\n", ktime_to_ns(total_time));

	return 0;
}


int thread_2(void *num) {
	int *count = (int*)num;
	int number = *count;
	int i, search_data;
	ktime_t start_time, end_time, total_time;
	struct my_node *current_node = NULL, *tmp;

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_add(&new->entry, &my_list);
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list insert time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(search_data = 0;search_data < number;search_data++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry(current_node, &my_list, entry) {
			if(current_node->data == search_data) {
				break;
			}
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list search time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry_safe(current_node, tmp, &my_list, entry) {
			list_del(&current_node->entry);
			kfree(current_node);
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list delete time : %llu ns\n", ktime_to_ns(total_time));

	return 0;
}


int thread_3(void *num) {
	int *count = (int*)num;
	int number = *count;
	int i, search_data;
	ktime_t start_time, end_time, total_time;
	struct my_node *current_node = NULL, *tmp;

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_add(&new->entry, &my_list);
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list insert time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(search_data = 0;search_data < number;search_data++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry(current_node, &my_list, entry) {
			if(current_node->data == search_data) {
				break;
			}
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list search time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry_safe(current_node, tmp, &my_list, entry) {
			list_del(&current_node->entry);
			kfree(current_node);
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list delete time : %llu ns\n", ktime_to_ns(total_time));

	return 0;
}


int thread_4(void *num) {
	int *count = (int*)num;
	int number = *count;
	int i, search_data;
	ktime_t start_time, end_time, total_time;
	struct my_node *current_node = NULL, *tmp;

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_add(&new->entry, &my_list);
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list insert time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(search_data = 0;search_data < number;search_data++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry(current_node, &my_list, entry) {
			if(current_node->data == search_data) {
				break;
			}
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list search time : %llu ns\n", ktime_to_ns(total_time));

	start_time = 0;
	end_time = 0;
	total_time = 0;

	for(i = 0;i < number;i++) {
		start_time = ktime_get();
		spin_lock(&counter_lock);
		list_for_each_entry_safe(current_node, tmp, &my_list, entry) {
			list_del(&current_node->entry);
			kfree(current_node);
		}
		spin_unlock(&counter_lock);
		end_time = ktime_get();
		total_time += ktime_sub(end_time, start_time);
	}

	printk("Spinlock linked list delete time : %llu ns\n", ktime_to_ns(total_time));

	return 0;
}
