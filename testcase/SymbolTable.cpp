#include "SymbolTable.h"

void SymbolTable::run(string filename) {
	int code = 0; 
	int i=0;
	int num_comp=0;
	int num_splay=0;
	ifstream ifs;
	ifs.open(filename, ifstream::in);
	
	symbolTree *symptr = new symbolTree();
	symbolTree *t = new symbolTree();
	
	string lenh, name, type, value, status;
	while(ifs.good()){
		lenh = ""; name = ""; type = ""; value = ""; status = "";
		ifs >> lenh;
		num_comp = 0;
		num_splay = 0;
		if (lenh == "INSERT")	code=1;
		else if (lenh == "ASSIGN")	{
			code=2;
			ifs >> name; 
			string s;
			getline(ifs, s);
			int start = s.find(" ") + 1;
			value = s.substr(start, s.length());
		}	
		else if (lenh == "BEGIN")	code=3;
		else if (lenh == "END")		code=4;
		else if (lenh == "LOOKUP")	code=5;
		else if (lenh == "PRINT")	code=6;
		else if (lenh == "RPRINT")	code=7;
		else {
			string s;
			getline(ifs, s);
			string err = lenh + s;
			throw InvalidInstruction(err);
		}
		switch (code) {
			case 1:
			{
				string err = lenh + " " + name + " " + type + " " + status;
				int KQ =0;
				ifs >> name; ifs >> type; ifs >> status;
				if(symptr->is_Valid_name(name) == 1){
					throw InvalidInstruction(err);
				} 
				if(type.find('>') != std::string::npos && status == "false" && i != 0){
					throw InvalidInstruction(err);
				}
				else if (status == "true"){
					KQ = symptr->insert(t, t->new_node(name,type, 0), num_comp, num_splay);
				} else if(status == "false"){
					KQ = symptr->insert(t, t->new_node(name,type, i), num_comp, num_splay);
				}
				if(KQ==0){
					throw Redeclared(err);
				}
				else if(KQ==1){
					cout << num_comp << " " << num_splay;
				}
				break;
			}
			case 2:
			{
				string err = lenh + " " + name + " " + value;
				int g = -1;
				if(symptr->is_Valid_name(name) == 1){
					throw InvalidInstruction(err);
				}
				if(value.find_first_of("(),") != std::string::npos){
					string f_name = value.substr(0, value.find('('));
					symbol *node = symptr->find_name(t, t->root, f_name);
					if(node != NULL){
						g = symptr->is_Valid_type(node, value);
					}
				} 
				int k = -1;
				k = symptr->lookup(t, t->root, name, num_splay, num_comp);
				if(g == 1){
					if(k == 0){
						throw Undeclared(err);
					}
					t->root->value = value;
					cout << num_comp << " " << num_splay;
				}else if(g == 0){
					throw TypeMismatch(err);
				} else {
					if(k==1){
						k = symptr->assign(t, t->root, name, value);
					} else if (k==0){
						throw Undeclared(err);
					}
					if(k==0) {
						throw TypeMismatch(err);
					} else if(k==1){
						cout << num_comp << " " << num_splay;
					}
				}
				break;
			}
			case 3:
			{
				i = i+1;
				break;	
			}				
			case 4:
			{
				symptr->inorder(t, t->root, i, num_splay);
				i = i-1;
				break;
			}
			case 5:
			{
				ifs >> name;
				string err = lenh + " " + name;
				int l = symptr->lookup(t, t->root, name, num_splay, num_comp);
				if (l == 0){
					throw Undeclared(err);
				}else if(l == 1) cout << t->root->level;
				break;
			}
			case 6:
				symptr->preorder(t, t->root);
				break;
			//case 7:
		}
		if(ifs.eof() != 1 && code != 3 && code != 4) cout << endl;
	}
	if (i > 0) {
		cout << endl;
		throw UnclosedBlock(i);
	} else if (i < 0) throw UnknownBlock();
		
	delete symptr;
	delete t;
	ifs.close();
}

symbol *symbolTree::new_node(string name, string type, int level)
{
	symbol *n = new symbol(name, type, level);
	return n;
}

void symbolTree::left_rotate(symbolTree *t, symbol *x)
{
	symbol *y = x->right;
	x->right = y->left;
	if(y->left != NULL) {
	  y->left->parent = x;
	}
	y->parent = x->parent;
	if(x->parent == NULL) { //x is root
	  t->root = y;
	}
	else if(x == x->parent->left) { //x is left child
	  x->parent->left = y;
	}
	else { //x is right child
	  x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

void symbolTree::right_rotate(symbolTree *t, symbol *x)
{
	symbol *y = x->left;
	x->left = y->right;
	if(y->right != NULL) {
	  y->right->parent = x;
	}
	y->parent = x->parent;
	if(x->parent == NULL) { //x is root
	  t->root = y;
	}
	else if(x == x->parent->right) { //x is left child
	  x->parent->right = y;
	}
	else { //x is right child
	  x->parent->left = y;
	}
	y->right = x;
	x->parent = y;
}

void symbolTree::splay(symbolTree *t, symbol *n, int &num_splay)
{
	while(n->parent != NULL) { //node is not root
	  if(n->parent == t->root) { //node is child of root, one rotation
		if(n == n->parent->left) {
		  right_rotate(t, n->parent);
		}
		else {
		  left_rotate(t, n->parent);
		}
	  }
	  else {
		symbol *p = n->parent;
		symbol *g = p->parent; //grandparent

		if(n->parent->left == n && p->parent->left == p) { //both are left children
		  right_rotate(t, g);
		  right_rotate(t, p);
		}
		else if(n->parent->right == n && p->parent->right == p) { //both are right children
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
	  num_splay++;
	}
}

int symbolTree::insert(symbolTree *t, symbol *n, int &num_comp, int &num_splay)
{
	symbol *y = NULL;
	symbol *temp = t->root;
	int rlt = 0;
	while(temp != NULL) {
		y = temp;
		if(n->level < temp->level)
			temp = temp->left;
		else if(n->level > temp->level)
			temp = temp->right;
		else if(n->level == temp->level){
			if((n->name).compare(temp->name) < 0)
				temp = temp->left;
			else if((n->name).compare(temp->name) > 0)
				temp = temp->right;
			else if((n->name).compare(temp->name) == 0){
				throw Redeclared(n->name);
			}
		}
		num_comp++;
	}
	n->parent = y;
	
	
	if(y == NULL){ //newly added node is root
		t->root = n;
		rlt=1;
	}
	else if(n->level < y->level){
		y->left = n;
		rlt=1;
	}
	else if(n->level > y->level){
		y->right = n;
		rlt=1;
	}
	else if(n->level == y->level){
		int i = this->insert_name(t, n, y);
		if(i==1) rlt=1;
		else rlt=2;
	}
	
	splay(t, n, num_splay);
	return rlt;
}

int symbolTree::insert_name(symbolTree *t, symbol *n, symbol *y)
{
	int m =0;
	if(y == NULL){ //newly added node is root
		t->root = n;
		m=1;
	}
	else if((n->name).compare(y->name) < 0){
		y->left = n;
		m=1;
	}
	else if((n->name).compare(y->name) > 0){
		y->right = n;
		m=1;
	}
	else if((n->name).compare(y->name) == 0){
		m=0;
	}
	return m;
}

int symbolTree::lookup(symbolTree *t, symbol *n, string name, int &num_splay, int &num_comp)
{
	int l = -1;
	if(n->name == name){
		l = 1;
	}else {
		n = t->root->right;
		while(n){
			if(n->left == NULL && n->right == NULL) break;
			if(n->name == name){
				this->splay(t, n, num_splay);
				l = 1;
				break;
			}
			if(name.compare(n->name) < 0){
				if(n->left) n = n->left;
				else n = n->right;
			}
			if(name.compare(n->name) > 0){
				if(n->right) n = n->right;
				else n = n->left;
			}
			num_comp++;
		}
		n = t->root->left;
		while(n){
			if(n->left == NULL && n->right == NULL) break;
			if(n->name == name){
				this->splay(t, n, num_splay);
				l = 1;
				break;
			}
			if(name.compare(n->name) < 0){
				if(n->left) n = n->left;
				else n = n->right;
			}
			if(name.compare(n->name) > 0){
				if(n->right) n = n->right;
				else n = n->left;
			}
			num_comp++;
		}
	}
	if(n->name == name) l = 1;
	else l = 0;
	return l;
}

symbol *symbolTree::find_name(symbolTree *t, symbol *n, string name)
{
	symbol *m = NULL;
	if(n->name == name){
		m = n;
	}else {
		n = t->root->right;
		while(n){
			if(n->left == NULL && n->right == NULL) break;
			if(n->name == name){
				m = n;
				break;
			}
			if(name.compare(n->name) < 0){
				if(n->left) n = n->left;
				else n = n->right;
			}
			if(name.compare(n->name) > 0){
				if(n->right) n = n->right;
				else n = n->left;
			}
		}
		n = t->root->left;
		while(n){
			if(n->left == NULL && n->right == NULL) break;
			if(n->name == name){
				m = n;
				break;
			}
			if(name.compare(n->name) < 0){
				if(n->left) n = n->left;
				else n = n->right;
			}
			if(name.compare(n->name) > 0){
				if(n->right) n = n->right;
				else n = n->left;
			}
		}
	}
	if(n->name == name) m = n;
	else m = NULL;
	return m;
}

void symbolTree::inorder(symbolTree *t, symbol *n, int &i, int &num_splay) {
	if(n != NULL) {
		if(n->level == i){
			this->delete_symbol(t, n, num_splay);
		} 
		inorder(t, n->left, i, num_splay);
		inorder(t, n->right, i, num_splay);
	}
}

void symbolTree::preorder(symbolTree *t, symbol *n) {
	if(n != NULL) {
		cout << n->name << "//" << n->level << " ";
		preorder(t, n->left);
		preorder(t, n->right);
	}
}

symbol *symbolTree::maximum(symbolTree *t, symbol *x) {
	while(x->right != NULL)
		x = x->right;
	return x;
}

void symbolTree::delete_symbol(symbolTree *t, symbol *n, int &num_splay) {
	splay(t, n, num_splay);

	symbolTree *left_subtree =  new symbolTree();
	left_subtree->root = t->root->left;
	if(left_subtree->root != NULL)
		left_subtree->root->parent = NULL;

	symbolTree *right_subtree = new symbolTree();
	right_subtree->root = t->root->right;
	if(right_subtree->root != NULL)
		right_subtree->root->parent = NULL;

	delete(n);

	if(left_subtree->root != NULL) {
		symbol *m = maximum(left_subtree, left_subtree->root);
		splay(left_subtree, m, num_splay);
		left_subtree->root->right = right_subtree->root;
		t->root = left_subtree->root;
	}
	else {
		t->root = right_subtree->root;
	}
}

bool symbolTree::isNumber(const string& str)
{
    for (char const &c : str) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}

int symbolTree::isString(string str)
{
	if(str.find("'") == 0){
		if(str.find("'", 1) == (str.length()-1)){
			 if (str.empty())
			 {
				return 1;
			 }
			 else if(str.find_first_not_of("a'bcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos)
			 {
				return 1;
			 }
			 else
			 {
				return 0;
			 }
		} else return 1;
	} else return 1;
}

int symbolTree::isOtherSymbol(string str)
{	
	int rlt=0;
	if (str.find("'") == std::string::npos){
		if(str.find_first_not_of("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") == std::string::npos){
			rlt = 1;
		} else rlt = 0;
	}
	return rlt;
}

int symbolTree::assign(symbolTree *t, symbol *n, string name, string value)
{
	int rlt = 0;
	if(n->name == name){
		if(n->type == "number" && isNumber(value) == 1){
			n->value = value;
			rlt = 1;
		} else if(n->type == "number" && isNumber(value) != 1){
			rlt = 0;
		} else if(n->type == "string" && isString(value) == 0){
			n->value = value;
			rlt = 1;
		} else if(n->type == "string" && isString(value) == 1){
			rlt = 0;
		}else{
			
		}
	}
	return rlt;
}

int symbolTree::is_Valid_type(symbol *n, string value){
	int rlt = -1;
	string type_a = (n->type).substr(1, (n->type).find(',')-1);
	string type_b = (n->type).substr((n->type).find(',')+1, (n->type).find(',')-1);
	string tmp_a = value.substr(value.find("(")+1);
	string tmp_b = value.substr(value.find(',')+1);
	string value_a = tmp_a.substr(0, tmp_a.find(','));
	string value_b = tmp_b.substr(0, tmp_b.find(')'));
	if(type_a == "number" && isNumber(value_a) == 1){
		if(type_b == "number" && isNumber(value_b) == 1){
			n->value = value;
			rlt = 1;
		}else if(type_b == "number" && isNumber(value_b) !=1){
			rlt = 0;
		}else if(type_b == "string" && is_Valid_name(value_b) == 0){
			n->value = value;
			rlt = 1;
		}else if(type_b == "string" && is_Valid_name(value_b) == 1){
			rlt =0;
		}
	}else if(type_a == "number" && isNumber(value_a) != 1){
		rlt = 0;
	} else if(type_a == "string" && is_Valid_name(value_a) == 0){
		if(type_b == "number" && isNumber(value_b) == 1){
			n->value = value;
			rlt = 1;
		}else if(type_b == "number" && isNumber(value_b) !=1){
			rlt = 0;
		}else if(type_b == "string" && is_Valid_name(value_b) == 0){
			n->value = value;
			rlt = 1;
		}else if(type_b == "string" && is_Valid_name(value_b) == 1){
			rlt =0;
		}
	} else if(type_a == "string" && is_Valid_name(value_a) == 1){
		rlt = 0;
	}
	return rlt;
}

int symbolTree::is_Valid_name(string name){
	string first = name.substr(0,1);
	if(first.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos ) return 1;
	if(name.find_first_not_of("a'bcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos){
		return 1;
	}
	return 0;
}