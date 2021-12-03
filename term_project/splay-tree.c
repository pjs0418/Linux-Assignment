#include<stdio.h>
#include<stdlib.h>
#include<time.h>

struct node {
	int key;
	struct node *left, *right;
};

struct node* newNode(int key) {
	struct node* node = (struct node*)malloc(sizeof(struct node));
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

void preOrder(struct node *root) {
	if (root != NULL) {
		printf("%d ", root->key);
		preOrder(root->left);
		preOrder(root->right);
	}
}

int main() {
	struct timespec begin, end;
	struct node *root = newNode(-1);
	long time;
	int i;

	/*root->left = newNode(50);
	root->right = newNode(200);
	root->left->left = newNode(40);
	root->left->left->left = newNode(30);
	root->left->left->left->left = newNode(20);*/

	clock_gettime(CLOCK_MONOTONIC, &begin);
	for(i = 0;i < 100000;i++) {
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
}
