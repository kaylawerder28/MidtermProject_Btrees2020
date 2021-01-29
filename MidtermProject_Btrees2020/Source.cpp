#include <iostream>
#include <string>
using namespace std;


//Midterm Project
class Person {
private:
	string first, last; //creating string for first and last name of person
	int age;
	double height, weight;
public:
	Person() { first = ""; last = ""; age = 0; } //empty constructor
	Person(string f, string l, int a, double h, double w) { this->first = f; last = l; age = a; height = h; weight = w; } // full constructor
	void setFirstName(string f) { first = f; }
	void setLastName(string l) { last = l; }
	void setAge(int a) { age = a; }
	void setHeight(double h) { height = h; }
	void setWeight(double w) { weight = w; }
	string getFirstName() { return first; }
	string getLastName() { return last; }
	int getAge() { return age; }
	double getHeight() { return height; }
	double getWeight() { return weight; }
};


class Node
{
public:
	Person * keys; // an array of keys for the person class
	Node ** Child; // array of child pointers in the B Tree node class
	int nk;	// minimum range for number of keys
	int k;	// number of keys
	bool leaf; // if node is a leaf = true, if not = false


	Node(int n, bool leaf2);	// full constructor

	//functions for searching
	void traverse();	// traverses through the tree
	Node *search(Person x) {
		int y = 0;
		while (y < k && keys[y].getFirstName() < x.getFirstName()) { y++; } // locate key greater/equal to x 
		if (keys[y].getFirstName() == x.getFirstName()) return this; // if key we are searching for is equal to x
		if (leaf == true)return NULL; // if key we are searching for is not in the tree & the value is a leaf
		return Child[y]->search(x); // search for the child
	}
	// searches a key in the subtree, returns null if value is not in the tree
	int findSpecificKey(Person x); //returns  the index of the first node found that is greater/equal to the key we are adding (x)

	//functions for insertion
	void insertOpen(Person x); // function is called if the new key added is not full -> there is an open location
	void split(int y, Node *z); // split the child z ONLY when the child is full
	Person previous(int index); // gets the previous value of the key
	Person after(int index); // gets the value after the key
	void Insert(int index); // places the value to fill the child node in that specific position


	//functions for deletion
	void removeKey(Person x); //remove the key with that specific node
	void removeLeaf(int index); // remove a key at a specific position/index
	void removeOther(int index); // remove a non-leaf key at a specific position/index
	void borrow(int index); // borrows a key from the child's location-1, used in deletion for balancing
	void borrowNew(int index); //borrows a key from the child's location+1, used in deletion for balancing
	void combine(int index); //merges the keys once deletion is complete


};

class BTree {

public:
	Node *root; //pointer to root
	int nk; // minimum degree

	BTree(int nk2) {
		root = NULL; // initializes tree as empty
		nk = nk2;
	}
	void traverse() {
		if (root != NULL) root->traverse();
	}

	Node *search(Person x) {
		return(root == NULL) ? NULL : root->search(x);
	}

	void insertValue(Person x); // inserts new value into tree

	void removeKey(Person x); // deletes new value in tree

};

Node::Node(int nk3, bool leaf3)
{
	nk = nk3; //min degree
	leaf = leaf3; //leaf
	keys = new Person[2 * nk - 1]; //max number of keys that can exist
	Child = new Node *[2 * nk]; // max number of child pointers that can exist

	k = 0; // # of keys
}

int Node::findSpecificKey(Person x)
{
	int index = 0;
	while (index < k && keys[index].getFirstName() < x.getFirstName())
		++index;
	return index;
}

void Node::traverse() {
	int x;
	for (x = 0; x < k; x++) // k total keys
	{
		if (leaf == false) { Child[x]->traverse(); }
		cout << "First name: " << keys[x].getFirstName() << ", ";
		cout << "Last name: " << keys[x].getLastName() << ", ";
		cout << "Age: " << keys[x].getAge() << ", ";
		cout << "Height: " << keys[x].getHeight() << ", ";
		cout << "Weight: " << keys[x].getWeight() << endl;
	}
	if (leaf == false)
	{
		Child[x]->traverse(); // this will output the tree 
	}
}


void Node::insertOpen(Person x) {
	int i = k - 1; // this sets i = rightmost value in the tree's index
	if (leaf == true) // if x is a leaf node
	{
		while (i >= 0 && keys[i].getFirstName() > x.getFirstName())
		{
			keys[i + 1] = keys[i];
			i--;
		}
		keys[i + 1] = x;
		k++; // increment max number of keys
	}
	else { //if x is not a leaf
		while (i >= 0 && keys[i].getFirstName() > x.getFirstName())  i--;

		//check is child is empty or full 
		if (Child[i + 1]->k == 2 * nk - 1)
		{
			split(i + 1, Child[i + 1]); // if the child has no more room, split keys

			if (keys[i + 1].getFirstName() < x.getFirstName())
				i++;
		}
		Child[i + 1]->insertOpen(x);
	}
}

void Node::split(int y, Node *z) {
	Node * node = new Node(z->nk, z->leaf);
	node->k = nk - 1; //node can hold nk-1 keys total

	for (int i = 0; i < nk - 1; i++) { // used to transfer the t-1 keys to node
		node->keys[i] = z->keys[i + nk];
	}

	if (z->leaf == false) {
		for (int j = 0; j < nk; j++) { // used to copy last t key to node
			node->Child[j] = z->Child[j + nk];
		}
	}

	z->k = nk - 1; // keys is now -1

	for (int g = k; g >= y + 1; g--) {
		Child[g + 1] = Child[g]; // new child node
	}

	Child[y + 1] = node; // node=new child

	for (int h = k - 1; h >= y; h--) { //moves all keys up 1 to insert
		keys[h + 1] = keys[h];
	}

	keys[y] = z->keys[nk - 1]; //puts the middle key in the node
	k++; // add 1 to keys

}

Person Node::previous(int index) {
	Node* node = Child[index]; //until we hit a leaf
	while (!node->leaf) node = node->Child[node->k];
	return node->keys[node->k - 1]; //returns last key
}

Person Node::after(int index) {
	Node* node = Child[index]; //until we hit a leaf
	while (!node->leaf) node = node->Child[0];
	return node->keys[0]; //returns first key (index=0)
}

void Node::Insert(int index) {
	if (index != 0 && Child[index - 1]->k >= nk) //if the index is not zero and the childs index-1 is greater than the total # of keys
		borrow(index); //borrow a key from the key before if it has more than index-1
	else if (index != k && Child[index + 1]->k >= nk)
		borrowNew(index); // borrow a key from next child if it has more than index-1
	else //combine the 2 nodes together with previous/next sibling now
	{
		if (index != k) combine(index); // previous
		else combine(index - 1);//next
	}
	return;
}

void Node::removeKey(Person x) {
	int index = findSpecificKey(x); //find the key to be removed

	if (index < k && keys[index].getFirstName() == x.getFirstName()) //if the key is in the tree
	{
		if (leaf)removeLeaf(index); //if specific key to be deleted is a leaf call that function
		else removeOther(index); //if specific key to be deleted is not a leaf call that function
	}
	else // the key is not in the tree
	{
		if (leaf) {
			cout << "The key " << x.getFirstName() << " cannot be found in the current tree." << endl;
			return;
		}



		bool check;
		if (index == k) { // if the key is in the subtree
			check = true;
		}
		else  check = false;

		if (check && index > k) Child[index - 1]->removeKey(x);
		else Child[index]->removeKey(x);
	}
	return;
}

void Node::removeLeaf(int index) {
	for (int i = index + 1; i < k; i++)  //to delete a leaf 
		keys[i - 1] = keys[i];
	k--; // subtract 1 key
	return;
}

void Node::removeOther(int index) {
	Person x = keys[index];

	if (Child[index + 1]->k >= nk)// if index+1 is greater than total number of keys
	{
		Person afterKey = after(index);
		keys[index] = afterKey;
		Child[index + 1]->removeKey(afterKey);
	}

	else if (Child[index]->k >= nk) //if index is greater than total number of keys
	{
		Person beforeKey = previous(index);
		keys[index] = beforeKey;
		Child[index]->removeKey(beforeKey);
	}

	else {
		combine(index);
		Child[index]->removeKey(x);
	}
	return;
}

void Node::borrow(int index) {
	Node *childNode = Child[index];
	Node *siblingofChild = Child[index - 1];

	for (int i = childNode->k - 1; i >= 0; --i) { // moving child[key] up 1 memory location
		childNode->keys[i + 1] = childNode->keys[i];
	}

	if (!childNode->leaf) { //child[index] is not leaf
		for (int i = childNode->k; i >= 0; --i) {
			childNode->Child[i + 1] = childNode->Child[i];
		}
	}

	childNode->keys[0] = keys[index - 1]; //child's first key = key[index-1]

	if (!childNode->leaf) {
		childNode->Child[0] = siblingofChild->Child[siblingofChild->k]; //sibling's last child is now Child[index] first child
	}

	keys[index - 1] = siblingofChild->keys[siblingofChild->k - 1]; //reduces # of keys in sibling by 1

	childNode->k = childNode->k + 1; //increasing child count
	siblingofChild->k = siblingofChild->k + 1; // increasing sibling count

	return;
}

void Node::borrowNew(int index) {
	Node *childNode = Child[index];
	Node *siblingofChild = Child[index + 1];

	childNode->keys[(childNode->k)] = keys[index]; //keys inserted as last child

	if (!childNode->leaf) {
		childNode->Child[(childNode->k) + 1] = siblingofChild->Child[0]; //last child of Child is sibling's first
	}

	keys[index] = siblingofChild->keys[0]; //key now has first key from sibling

	for (int i = 1; i < siblingofChild->k; ++i) {
		siblingofChild->keys[i - 1] = siblingofChild->keys[i]; //move all keys back 1 location
	}

	if (!siblingofChild->leaf) {
		for (int i = 1; i < siblingofChild->k; ++i) {
			siblingofChild->keys[i - 1] = siblingofChild->keys[i]; // moving pointer of child 1 location back
		}
	}

	childNode->k = childNode->k + 1; // increasing child count
	siblingofChild->k = siblingofChild->k - 1; // decreasing sibling count

	return;
}

void Node::combine(int index) {
	Node *childNode = Child[index];
	Node *siblingofChild = Child[index + 1];

	childNode->keys[nk - 1] = keys[index]; // copying key into child node

	for (int i = 0; i < siblingofChild->k; ++i) {
		childNode->keys[i + nk] = siblingofChild->keys[i]; //copying sibling into child
	}

	if (!childNode->leaf) {
		for (int i = 0; i <= siblingofChild->k; ++i) {
			childNode->Child[i + nk] = siblingofChild->Child[i]; //putting child points into child node
		}
	}

	for (int i = index + 1; i < k; ++i) {
		keys[i - 1] = keys[i]; //moving each key 1 location
	}

	for (int i = index + 2; i <= k; ++i) {
		Child[i - 1] = Child[i]; //moving child pointers
	}

	childNode->k += siblingofChild->k + 1;
	k--; //decreasing key count

	delete(siblingofChild);
	return;

}

void BTree::insertValue(Person x) {
	if (!root) {
		root = new Node(nk, true); //create root
		root->keys[0] = x; //insert the first key
		root->k = 1; //total number of keys allowed 
	}
	else {
		if (root->k == 2 * nk - 1) {  //if the root is full (2*# of keys-1)
			Node *node = new Node(nk, false); //create new root
			node->Child[0] = root; // make new root the parent of the old root
			node->split(0, root);  //split & move 1 key to new root created

			int i = 0;
			if (node->keys[0].getFirstName() < x.getFirstName()) {
				i++; //root has 2 children, use this to place the next child in the correct location
			}
			node->Child[i]->insertOpen(x);

			root = node; //change the root to the new root
		}
		else {
			root->insertOpen(x); //if the root is not full & has room for insertion
		}
	}
}


void BTree::removeKey(Person x) {
	if (!root) {
		cout << "Tree is empty." << endl;
	}

	root->removeKey(x);

	if (root->k == 0) {
		Node * node = root;
		if (root->leaf) root = NULL;
		else root = root->Child[0];

		delete node;
	}
	return;
}




int main()
{
	BTree tree(3); //create a tree of order 3
	// tree will print in order traversal

	Person person1("Alex", "Smith", 20, 6, 180);
	Person person2("Sally", "Shores", 30, 5, 125);
	Person person3("Joe", "Johnson", 50, 6.1, 200);
	Person person4("Gabby", "Wilson", 32, 5.5, 145);
	Person person5("Juan", "Garcia", 45, 5.8, 160);
	Person person6("Sue", "Wells", 10, 4.6, 70);
	Person person7("Tom", "Sanders", 60, 6, 175);
	Person person8("Jim", "Willis", 40, 6.3, 185);
	Person person9("Wallace", "Reeman", 50, 5.9, 170);
	Person person10("Kyle", "Marks", 24, 5.6, 150);
	Person person11("Brady", "Davidson", 23, 6, 175);
	Person person12("Louis", "Meyers", 63, 5.7, 155);
	Person person13("Sammy", "Shields", 18, 5.3, 125);
	Person person14("Phillip", "Shields", 40, 5.8, 165);
	Person person15("Reese", "Paulers", 30, 6, 180);

	tree.insertValue(person1);
	tree.insertValue(person2);
	tree.insertValue(person3);
	tree.insertValue(person4);
	tree.insertValue(person5);
	tree.insertValue(person6);
	tree.insertValue(person7);
	tree.insertValue(person8);
	tree.insertValue(person9);
	tree.insertValue(person10);
	tree.insertValue(person11);
	tree.insertValue(person12);
	tree.insertValue(person13);
	tree.insertValue(person14);
	tree.insertValue(person15);

	cout << "Traversal of the tree is: " << endl;
	tree.traverse();

	cout << endl;
	cout << endl;

	tree.removeKey(person3);
	cout << "The tree after removing Joe Johnson: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person6);
	cout << "The tree after removing Sue Wells: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person10);
	cout << "The tree after removing Kyle Marks: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person13);
	cout << "The tree after removing Sammy Shields: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;


	tree.removeKey(person15);
	cout << "The tree after removing Reese Paulers: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person6);
	//this should output that the key is not in the tree because it was already deleted
	cout << endl;
	cout << endl;

	tree.removeKey(person1);
	cout << "The tree after removing Alex Smith: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person4);
	cout << "The tree after removing Gabby Wilson: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person5);
	cout << "The tree after removing Juan Garcia: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;

	tree.removeKey(person14);
	cout << "The tree after removing Phillip Shields: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;


	tree.removeKey(person9);
	cout << "The tree after removing Wallace Reeman: " << endl;
	tree.traverse();
	cout << endl;
	cout << endl;


	cout << "Thank you for testing B-trees! Good-bye" << endl;



	system("pause");
	return 0;
}