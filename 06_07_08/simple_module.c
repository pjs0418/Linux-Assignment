#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/rbtree.h>

#define FALSE 0
#define TRUE 1
#define BILLION 1000000000

int size[3] = {1000, 10000, 100000};

void struct_example(void);
void rb_example(void);
unsigned long long calclock3(struct timespec *spclock, unsigned long long *total_time, unsigned long long *total_count);

struct my_node {
	struct list_head entry;
	int data;
};

struct my_type {
	struct rb_node node;
	int key;
	int value;
};

int __init simple_module_init(void)
{
	rb_example();
	printk("Hello Module!\n");
	return 0;
}

void __exit simple_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(simple_module_init);
module_exit(simple_module_cleanup);
MODULE_LICENSE("GPL");

void struct_example(void)
{
	int i, j, search_data;
	struct list_head my_list;
	struct timespec spclock[2];
	struct my_node *current_node, *tmp;
	unsigned long long total_time, total_count;

	for(i = 0;i < 3;i++){
		printk("size = %d\n", size[i]);
		
		current_node = NULL;
		total_time = 0;
		total_count = 0;

		INIT_LIST_HEAD(&my_list);

		for(j = 0;j < size[i];j++) {
			struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
			new->data = j;
			ktime_get_ts(&spclock[0]);
			list_add(&new->entry, &my_list);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("add_total_time: %llu, count: %llu\n", total_time, total_count);
		/*list_for_each_entry(current_node, &my_list, entry) {
			printk("current value: %d\n", current_node->data);
		}*/

		total_time = 0;
		total_count = 0;

		for(search_data = 0;search_data < size[i];search_data++) {
			ktime_get_ts(&spclock[0]);
			list_for_each_entry(current_node, &my_list, entry) {
				if(current_node->data == search_data) {
					break;
				}
			}
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("search_total_time: %llu, count: %llu\n", total_time, total_count);

		total_time = 0;
		total_count = 0;

		for(j = 0;j < size[i];j++) {
			list_for_each_entry_safe(current_node, tmp, &my_list, entry) {
				ktime_get_ts(&spclock[0]);
				list_del(&current_node->entry);
				kfree(current_node);
				ktime_get_ts(&spclock[1]);
				calclock3(spclock, &total_time, &total_count);
			}
		}

		printk("delete_total_time: %llu, count: %llu\n", total_time, total_count);
	}
}

unsigned long long calclock3(struct timespec *spclock, unsigned long long *total_time, unsigned long long *total_count)
{
	long temp, temp_n;
	unsigned long long timedelay = 0;

	if(spclock[1].tv_nsec >= spclock[0].tv_nsec)
	{
		temp = spclock[1].tv_sec - spclock[0].tv_sec;
		temp_n = spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = BILLION * temp + temp_n;
	}
	else
	{
		temp = spclock[1].tv_sec - spclock[0].tv_sec - 1;
		temp_n = BILLION + spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = BILLION * temp + temp_n;
	}

	__sync_fetch_and_add(total_time, timedelay);
	__sync_fetch_and_add(total_count, 1);
	
	return timedelay;
}

int rb_insert(struct rb_root *root, struct my_type *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while(*new) {
		struct my_type *this = container_of(*new, struct my_type, node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if(this->key < data->key)
			new = &((*new)->rb_right);
		else
			return 1;
	}

	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return 0;
}

struct my_type *rb_search(struct rb_root *root, int key)
{
	struct rb_node *node = root->rb_node;

	while(node) {
		struct my_type *data = container_of(node, struct my_type, node);

		if(data->key > key)
			node = node->rb_left;
		else if(data->key < key)
			node = node->rb_right;
		else
			return data;
	}

	return NULL;
}

int rb_delete(struct rb_root *mytree, int key)
{
	struct my_type *data = rb_search(mytree, key);

	if(data) {
		rb_erase(&data->node, mytree);
		kfree(data);
		return 0;
	}
	return 1;
}

void rb_example(void)
{
	struct rb_root my_tree = RB_ROOT;
	int i, j, search_data;
	struct timespec spclock[2];
	unsigned long long total_time, total_count;

	total_time = 0;
	total_count = 0;

	for(i = 0;i < 3;i++) {
		printk("size = %d\n", size[i]);

		for(j = 0;j < size[i];j++) {	
			struct my_type *new = kmalloc(sizeof(struct my_type), GFP_KERNEL);	
			new->value = j*10;
			new->key = j;

			ktime_get_ts(&spclock[0]);
			rb_insert(&my_tree, new);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("insert_total_time: %llu, count: %llu\n", total_time, total_count);

		total_time = 0;
		total_count = 0;

		for(search_data = 0;search_data < size[i];search_data++) {
			ktime_get_ts(&spclock[0]);
			rb_search(&my_tree, search_data);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("search_total_time: %llu, count: %llu\n", total_time, total_count);

		total_time = 0;
		total_count = 0;

		for(j = 0;j < size[i];j++) {
			ktime_get_ts(&spclock[0]);
			rb_delete(&my_tree, j);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("delete_total_time: %llu, count: %llu\n", total_time, total_count);
	}
}

