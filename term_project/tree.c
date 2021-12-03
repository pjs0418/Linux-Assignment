#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>

#define BILLION 1000000000

int size[3] = {1000, 10000, 100000};

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

typedef struct node {
	int data;
	struct node *left, *right, *parent;
}node;

typedef struct splay_tree {
	struct node *root;
}splay_tree;

node* new_node(int data) {
	node *n = (node*)kmalloc(sizeof(node), GFP_KERNEL);
	n->data = data;
	n->parent = n->left = n->right = NULL;

	return n;
}

splay_tree* new_splay_tree(void) {
	splay_tree *t = (splay_tree*)kmalloc(sizeof(splay_tree), GFP_KERNEL);
	t->root = NULL;

	return t;
}

node* maximum(splay_tree *t, node *x) {
	while(x->right != NULL)
		x = x->right;
	
	return x;
}

void left_rotate(splay_tree *t, node *x) {
	node *y = x->right;
	x->right = y->left;
	
	if(y->left != NULL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if(x->parent == NULL) {
		t->root = y;
	}
	else if(x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

void right_rotate(splay_tree *t, node *x) {
	node *y = x->left;
	x->left = y->right;
	if(y->right != NULL) {
		y->right->parent = x;
	}
	y->parent = x->parent;
	if(x->parent == NULL) {
		t->root = y;
	}
	else if(x == x->parent->right) {
		x->parent->right = y;
	}
	else {
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;
}

void splay(splay_tree *t, node *n) {
	while(n->parent != NULL) {
		if(n->parent == t->root) {
			if(n == n->parent->left) {
				right_rotate(t, n->parent);
			}
			else {
				left_rotate(t, n->parent);
			}
		}
		else {
			node *p = n->parent;
			node *g = p->parent;

			if(n->parent->left == n && p->parent->left == p) {
				right_rotate(t, g);
				right_rotate(t, p);
			}
			else if(n->parent->right == n && p->parent->right == p) {
				left_rotate(t, g);
				left_rotate(t, p);
			}
			else if(n->parent->right == n && p->parent->left == p) {
				left_rotate(t, p);
				right_rotate(t, g);
			}
			else if(n->parent->left == n && p->parent->right == p) {
				right_rotate(t, p);
				left_rotate(t, g);
			}
		}
	}
}

void insert(splay_tree *t, node *n) {
	node *y = NULL;
	node *temp = t->root;

	while(temp != NULL) {
		y = temp;
		if(n->data < temp->data)
			temp = temp->left;
		else
			temp = temp->right;
	}
	n->parent = y;

	if(y == NULL)
		t->root = n;
	else if(n->data < y->data)
		y->left = n;
	else
		y->right = n;

	splay(t,n);
}

node* search(splay_tree *t, int key) {

	node *p = t->root;

	if(!p)
		return false;

	while(p) {
		if(key == p->data)
			break;

		if(key < p->data) {
			if(!p->left)
				break;
			p = p->left;
		}
		else {
			if(!p->right)
				break;
			p = p->right;
		}
	}

	splay(t, p);

	return t->root;
}

/*node* search(splay_tree *t, node *n, int x) {
	if(x == n->data) {
		splay(t, n);
		return n;
	}
	else if(x < n->data)
		return search(t, n->left, x);
	else if(x > n->data)
		return search(t, n->right, x);
	else
		return NULL;
}*/

void delete(splay_tree *t, node *n) {

	splay_tree *left_subtree = new_splay_tree();
	left_subtree->root = t->root->left;
	if(left_subtree->root != NULL)
		left_subtree->root->parent = NULL;

	splay_tree *right_subtree = new_splay_tree();
	right_subtree->root = t->root->right;
	if(right_subtree->root != NULL)
		right_subtree->root->parent = NULL;

	kfree(n);

	if(left_subtree->root != NULL) {
		node *m = maximum(left_subtree, left_subtree->root);
		splay(left_subtree, m);
		left_subtree->root->right = right_subtree->root;
		t->root = left_subtree->root;
	}
	else {
		t->root = right_subtree->root;
	}
}

/*struct node *insert(struct node *root, int k) {
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
}*/

void inorder(splay_tree *t, node *n) {
	if(n != NULL) {
		inorder(t, n->left);
		printk("%d\n", n->data);
		inorder(t, n->right);
	}
}
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
	struct timespec spclock[2];
	unsigned long long total_time, total_count;
	splay_tree *t = new_splay_tree();
	node *n;

	for(i = 0;i < 3;i++) {
		printk("size = %d\n", size[i]);

		total_time = 0;
		total_count = 0;
		
		for(j = 0;j < size[i];j++) {
			node * new = new_node(j);
			ktime_get_ts(&spclock[0]);
			insert(t, new);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("insert_total_time: %llu, count: %llu\n", total_time, total_count);
	
		total_time = 0;
		total_count = 0;

		for(search_data = 0;search_data < size[i];search_data++) {
			ktime_get_ts(&spclock[0]);
			search(t, search_data);
			ktime_get_ts(&spclock[1]);
			calclock3(spclock, &total_time, &total_count);
		}

		printk("search_total_time: %llu, count: %llu\n", total_time, total_count);
	
		total_time = 0;
		total_count = 0;

		for(j = 0;j < size[i];j++) {
			n = search(t, j);
			ktime_get_ts(&spclock[0]);
			delete(t, n);
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
