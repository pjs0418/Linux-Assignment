#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

#define BILLION 1000000000

int size[3] = {1000, 1000, 1000};

void st_example(void);
unsigned long long calclock3(struct timespec *spclock, unsigned long long *total_time, unsigned long long *total_count);

int __init test_init(void) {
	st_example();
	printk("test module\n");
	return 0;
}

void __exit test_cleanup(void) {
	printk("test end\n");
}

module_init(test_init);
module_exit(test_cleanup);
MODULE_LICENSE("GPL");

struct node {
	int key;
	struct node *left, *right;
};

struct node* newNode(int key) {
	struct node* node = (struct node*)kmalloc(sizeof(struct node), GFP_KERNEL);
	node->key = key;
	node->left = node->right = NULL;
	return(node);
}

//Right Rotate
struct node *rightRotate(struct node *x) {
	struct node *y = x->left;
	x->left = y->right;
	y->right = x;
	return y;
}

//Left Rotate
struct node *leftRotate(struct node *x) {
	struct node *y = x->right;
	x->right = y->left;
	y->left = x;
	return y;
}

struct node *splay(struct node *root, int key) {
	// Base case : root is NULL or key is present at root
	if(root == NULL || root->key == key)
		return root;

	// Key lies in left subtree
	if(root->key > key) {
		// Key is not in tree, DONE
		if(root->left == NULL)
			return root;

		// Left-Left
		if(root->left->key > key) {
			root->left->left = splay(root->left->left, key);

			root = rightRotate(root);
		}
		// Left-Right
		else if(root->left->key < key) {
			root->left->right = splay(root->left->right, key);

			if(root->left->right != NULL)
				root->left = leftRotate(root->left);
		}

		// Do second rotation for root
		return (root->left == NULL)? root: rightRotate(root);
	}
	// Key lies in right subtree
	else {
		// Key is not in tree, DONE
		if (root->right == NULL)
			return root;

		// Right-Left
		if (root->right->key > key) {
			root->right->left = splay(root->right->left, key);

			if(root->right->left != NULL)
				root->right = rightRotate(root->right);
		}
		// Right-Right
		else if (root->right->key < key) {
			root->right->right = splay(root->right->right, key);

			root = leftRotate(root);
		}

		// Do second rotation for root
		return (root->right == NULL)? root: leftRotate(root);
	}
}

// Function to insert a new key k in splay tree
struct node *insert(struct node *root, int k) {
	// Base Case : If tree is empty
	if (root == NULL)
		return newNode(k);

	// Bring the closest leaf node to root
	root = splay(root, k);

	// If key is already present, then return
	if (root->key == k)
		return root;

	// Otherwise allocate memory for new node
	struct node *newnode = newNode(k);

	// If root's key is greater, make root as right child of newnode and copy the left child of root to newnode
	if (root->key > k) {
		newnode->right = root;
		newnode->left = root->left;
		root->left = NULL;
	}
	// If root's key is smaller, make root as left child of newnode and copy the right child of root to newnode
	else {
		newnode->left = root;
		newnode->right = root->right;
		root->right = NULL;
	}

	return newnode;
}

// Function to search
struct node *search(struct node *root, int key) {
	return splay(root, key);
}

struct node* delete_key(struct node *root, int key) {
	struct node *temp;
	
	if(!root)
		return NULL;

	root = splay(root, key);

	if (key != root->key)
		return root;

	if (!root->left) {
		temp = root;
		root = root->right;
	}
	else {
		temp = root;

		root = splay(root->left, key);

		root->right = temp->right;
	}

	kfree(temp);

	return root;
}
/*void preOrder(struct node *root) {
	if (root != NULL) {
		printf("%d ", root->key);
		preOrder(root->left);
		preOrder(root->right);
	}
}*/

/*int main() {
	struct timespec begin, end;
	struct node *root = newNode(2000);
	long time;
	int i;

	root->left = newNode(50);
	root->right = newNode(200);
	root->left->left = newNode(40);
	root->left->left->left = newNode(30);
	root->left->left->left->left = newNode(20);

	clock_gettime(CLOCK_MONOTONIC, &begin);
	for(i = 0;i < 1000;i++) {
		root = insert(root, i);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	time = (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec);
	printf("Time (Nano): %ld\n", time);

	printf("Preorder traversal of the modified Splay tree is \n");
	preOrder(root);
	printf("\n");
	//root = search(root, 20);
	//preOrder(root);
	//printf("\n");
	return 0;
}*/

void st_example(void) {
	int i, j, search_data;
	struct node *root = newNode(0);
	struct timespec spclock[2];
	unsigned long long total_time, total_count;

	root->left = newNode(-1);
	root->right = newNode(1);

	for(i = 0;i < 3;i++) {
		printk("size = %d\n", size[i]);

		total_time = 0;
		total_count = 0;
		
		for(j = 0;j < size[i];j++) {
			ktime_get_ts(&spclock[0]);
			root = insert(root, j);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("insert_total_time: %llu, count: %llu\n", total_time, total_count);

		total_time = 0;
		total_count = 0;

		for(search_data = 0;search_data < size[i];search_data++) {
			ktime_get_ts(&spclock[0]);
			root = search(root, search_data);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("search_total_time: %llu, count: %llu\n", total_time, total_count);
	
		total_time = 0;
		total_count = 0;

		for(j = 0;j < size[i];j++) {
			ktime_get_ts(&spclock[0]);
			root = delete_key(root, j);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("delete_total_time: %llu, count: %llu\n", total_time, total_count);
	}
}

unsigned long long calclock3(struct timespec *spclock, unsigned long long *total_time, unsigned long long *total_count) {
	long temp, temp_n;
	unsigned long long timedelay = 0;

	if(spclock[1].tv_nsec >= spclock[0].tv_nsec) {
		temp = spclock[1].tv_sec - spclock[0].tv_sec;
		temp_n = spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = BILLION * temp + temp_n;
	}
	else {
		temp = spclock[1].tv_sec - spclock[0].tv_sec - 1;
		temp_n = BILLION + spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = BILLION * temp + temp_n;
	}

	__sync_fetch_and_add(total_time, timedelay);
	__sync_fetch_and_add(total_count, 1);

	return timedelay;
}
