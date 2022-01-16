#include "SymbolTable.h"

void SymbolTable::run(string filename) {
	int code = 0; 
	int i=0;
	int k = 0;
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
		if(k == 0 && lenh == "PRINT") continue;
		if(k>0 && lenh != "BEGIN" && lenh != "END") cout << endl;
		num_comp = 0;
		num_splay = 0;
		if (lenh == "INSERT")	{
			code=1;	
		}
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
				int KQ =0;
				ifs >> name; ifs >> type; ifs >> status;
				string err = lenh + " " + name + " " + type + " " + status;
				if(status != "true" && status != "false") {
					string err = lenh + " " + name + " " + type;
					throw InvalidInstruction(err);
				}
				if(symptr->is_Valid_name(name) == 1){
					throw InvalidDeclaration(err);
				} 
				if(type.find('>') != std::string::npos && status == "false" && i != 0){
					throw InvalidDeclaration(err);
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
				symbol *node = NULL;
				if(symptr->is_Valid_name(name) == 1){
					throw InvalidInstruction(err);
				}
				if(value.find_first_of("(),") != std::string::npos){
					string f_name = value.substr(0, value.find('('));
					if(symptr->is_Valid_name(f_name) == 0){
						node = symptr->find_name(t, t->root, f_name, i, num_comp, num_splay);
					} else throw InvalidInstruction(err);
					if(node != NULL){
						if((node->type).find_first_of(">") == std::string::npos) throw TypeMismatch(err);
						g = symptr->is_Valid_type(t, t->root, node, value, i, num_comp, num_splay);
					}else throw Undeclared(err);
				}
				string str, f_type;
				if( node != NULL) {
					str = node->type;
					int idx = (int)str.length();
					f_type = str.substr(idx-6, idx);
				}
				int k = -1;
				k = symptr->lookup(t, t->root, name, num_splay, num_comp, i);
				if(g == 1){
					if(k == 0){
						throw Undeclared(err);
					}
					int p = symptr->assign_func(t, t->root, name, value, f_type);
					if(p==1){
						cout << num_comp << " " << num_splay;
					}else throw TypeMismatch(err);
				}else if(g == 0){
					throw TypeMismatch(err);
				} else {
					int e;
					if(k==1){
						e = symptr->assign(t, t->root, name, value, num_comp, num_splay, i);
					} else if (k==0){
						throw Undeclared(err);
					}
					if(e==0) {
						throw TypeMismatch(err);
					} else if(e==1){
						cout << num_comp << " " << num_splay;
					}
				}
				node = NULL;
				delete node;
				break;
			}
			case 3:
			{
				i = i+1;
				break;	
			}				
			case 4:
			{
				if( i==0 || i<0) throw UnknownBlock();
				symbol *fnode = symptr->find_to_delete(t, t->root, i);
				while(fnode != NULL) {
					symptr->delete_symbol(t, fnode, num_splay);
					fnode = symptr->find_to_delete(t, t->root, i);
				}
				i = i-1;
				break;
			}
			case 5:
			{
				ifs >> name;
				string err = lenh + " " + name;
				int l = symptr->lookup(t, t->root, name, num_splay, num_comp, i);
				if (l == 0){
					throw Undeclared(err);
				}else if(l == 1) cout << t->root->level;
				break;
			}
			case 6:
				symptr->preorder(t, t->root);
				break;
		}
		k++;
	}
	if (i > 0) {
		cout << endl;
		throw UnclosedBlock(i);
	} else if (i < 0) throw UnknownBlock();
		
	cout << endl;
	delete symptr;
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

void symbolTree::splay(symbolTree *t, symbol *n, int &num_splay)
{
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
		symbol *p = n->parent;
		symbol *g = p->parent;

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
				return 0;
			}
		}
		num_comp++;
	}
	n->parent = y;
	
	
	if(y == NULL){
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
	if(y == NULL){
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

int symbolTree::lookup(symbolTree *t, symbol *n, string name, int &num_splay, int &num_comp, int level)
{
	int l = -1;
	if(n->name == name){
		l = 1;
		num_comp++;
	}else {
		num_comp++;
		n = t->root->left;
		if(n) {
			while(n){
				if(n->name == name){
					l=1;
					num_comp++;
					break;
				}
				num_comp++;
				if(n->left == NULL && n->right == NULL) break;
				else if(name.compare(n->name) < 0){
					if(n->left) n = n->left;
					else n = n->right;
				}
				else if(name.compare(n->name) > 0){
					if(n->right) n = n->right;
					else n = n->left;
				} else break;
			}
		} else n = NULL;
		if(n == NULL || n->name != name) {
			n = t->root->right;
			if(n){
				while(n){
					if(n->name == name){
						l=1;
						num_comp++;
						break;
					}
					num_comp++;
					if(n->left == NULL && n->right == NULL) break;
					else if(name.compare(n->name) < 0){
						if(n->left) n = n->left;
						else n = n->right;
					}
					else if(name.compare(n->name) > 0){
						if(n->right) n = n->right;
						else n = n->left;
					} else break;
				}
			} else n = NULL;
		}
	}
	if(n != NULL && n->name == name) {
		l = 1;
		splay(t, n, num_splay);
	}
	else l = 0;
	return l;
}

symbol *symbolTree::find_name(symbolTree *t, symbol *n, string name, int level, int &num_comp, int &num_splay)
{
	symbol *m = NULL;
	if(n->name == name){
		m = n;
		num_comp++;
	}else {
		num_comp++;
		n = t->root->left;
		if(n) {
			while(n){
				if(n->name == name){
					m = n;
					num_comp++;
					break;
				}
				num_comp++;
				if(n->left == NULL && n->right == NULL) break;
				else if(name.compare(n->name) < 0){
					if(n->left) n = n->left;
					else n = n->right;
				}
				else if(name.compare(n->name) > 0){
					if(n->right) n = n->right;
					else n = n->left;
				} else break;
			}
		} else n = NULL;
		if(n == NULL || n->name != name) {
			n = t->root->right;
			if(n){
				while(n){
					if(n->name == name){
						m = n;
						num_comp++;
						break;
					}
					num_comp++;
					if(n->left == NULL && n->right == NULL) break;
					else if(name.compare(n->name) < 0){
						if(n->left) n = n->left;
						else n = n->right;
					}
					else if(name.compare(n->name) > 0){
						if(n->right) n = n->right;
						else n = n->left;
					} else break;
				}
			} else n = NULL;
		}
	}
	if(n != NULL && n->name == name) {
		m = n;
		splay(t, m, num_splay);
	}
	else m = NULL;
	return m;
}

void symbolTree::preorder(symbolTree *t, symbol *n) {
	if(n != NULL) {
		cout << n->name << "//" << n->level << " ";
		preorder(t, n->left);
		preorder(t, n->right);
	}
}

void symbolTree::inorder(symbolTree *t, symbol *n) {
	if(n != NULL) {
		inorder(t, n->left);
		cout << n->name << "//" << n->level << " ";
		inorder(t, n->right);
	}
}

symbol *symbolTree::maximum(symbolTree *t, symbol *x) {
	if(x->right == NULL) return NULL;
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

	delete n;
	
	if(left_subtree->root != NULL) {
		symbol *m = maximum(left_subtree, left_subtree->root);
		if(m==NULL) {
			t->root = left_subtree->root;
		}else{
			splay(left_subtree, m, num_splay);
			left_subtree->root->right = right_subtree->root;
			t->root = left_subtree->root;
		}
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
	string ss = str.substr(0,2);
	if(str.find("'") == 0){
		if(str.find("'", 1) == (str.length()-1)){
			 if (str.empty())
			 {
				return 1;
			 }
			 else if(str.find_first_not_of("a'bcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890 ") != std::string::npos)
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

int symbolTree::assign(symbolTree *t, symbol *n, string name, string value, int &num_comp, int &num_splay, int level)
{
	int rlt = 0;
	if(n->name == name){
		if(n->type == "number" && isNumber(value) == 1){
			n->value = value;
			rlt = 1;
		} else if(n->type == "number" && isNumber(value) != 1){
			if (isOtherSymbol(value) == 1){
				rlt = 0;
				int u = lookup(t, t->root, value, num_splay, num_comp, level);
				if(u==1){
					if(n->type == t->root->type){
						n->value = t->root->value;
						rlt = 1;
					}
				}
			}
			else rlt = -1;
		} else if(n->type == "string" && isString(value) == 0){
			n->value = value;
			rlt = 1;
		} else if(n->type == "string" && isString(value) == 1){
			rlt = 0;
			if (isOtherSymbol(value) == 1){
				int u = lookup(t, t->root, value, num_splay, num_comp, level);
				if(u==1){
					if(n->type == t->root->type){
						n->value = t->root->value;
						rlt = 1;
					}
				}
			}else rlt = -1;
		}
	}
	return rlt;
}

int symbolTree::assign_func(symbolTree *t, symbol *n, string name, string value, string f_type)
{
	int rlt = 0;
	if(n->name == name){
		if(n->type == f_type){
			n->value = value;
			rlt = 1;
		}else{
			rlt = 0;
		}
	}
	return rlt;
}

int symbolTree::is_Valid_type(symbolTree *t, symbol *m, symbol *n, string value, int level, int &num_comp, int &num_splay){
	int rlt = 1;
	string type = (n->type).substr(1, (n->type).find(')')-1);
	string tmp = value.substr(0, value.find(')'));
	string val = (tmp).substr((tmp).find('(')+1);
	string arr[100];
	stringstream st(type);
	stringstream sv(val);
	int v = 0;
	while (sv.good()) {
        string substr;
        getline(sv, substr, ',');
		arr[v] = substr;
		v++;
    }
	int c = 0;
	while(st.good()){
		string substr;
		getline(st, substr, ',');
		c++;
	}
	if(c != v) return 0;
	c = 0;
	stringstream st_sub(type);
	while(st_sub.good()){
		string substr;
		getline(st_sub, substr, ',');
		if(substr == "number" &&  isNumber(arr[c]) != 1){
			rlt = 0;
			if(isOtherSymbol(arr[c])){
				symbol *q = find_name(t, m, arr[c], level, num_comp, num_splay);
				if(q){
					string sbstr = (q->type).substr((q->type).length()-6);
					if(sbstr == "number") rlt = 1;
				}
			}
			break;
		}else if(substr == "string" && isString(arr[c]) != 0){
			rlt = 0;
			if(isOtherSymbol(arr[c])){
				symbol *q = find_name(t, m, arr[c], level, num_comp, num_splay);
				if(q){
					string sbstr = (q->type).substr((q->type).length()-6);
					if(sbstr == "string") rlt = 1;
				}
			}
			break;
		} 
		c++;
	}
	return rlt;
}

int symbolTree::is_Valid_name(string name){
	string first = name.substr(0,1);
	if(name == "string" || name == "number") return 1;
	if(first.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos ) return 1;
	if(name.find_first_not_of("a'bcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890") != std::string::npos){
		return 1;
	}
	return 0;
}

symbol *symbolTree::find_to_delete(symbolTree *t, symbol *n, int level)
{
	symbol *m = NULL;
	if(n->level == level){
		m = n;
	}else {
		n = t->root->left;
		if(n) {
			while(n){
				if(n->level == level){
					m = n;
					break;
				}
				if(n->left == NULL && n->right == NULL) break;
				else if(level < n->level){
					if(n->left) n = n->left;
					else n = n->right;
				}
				else if(level > n->level){
					if(n->right) n = n->right;
					else n = n->left;
				} else break;
			}
		} else n = NULL;
		if(n == NULL || n->level != level) {
			n = t->root->right;
			if(n){
				while(n){
					if(n->level == level){
						m = n;
						break;
					}
					if(n->left == NULL && n->right == NULL) break;
					else if(level < n->level){
						if(n->left) n = n->left;
						else n = n->right;
					}
					else if(level > n->level){
						if(n->right) n = n->right;
						else n = n->left;
					} else break;
				}
			} else n = NULL;
		}
	}
	if(n != NULL && n->level == level) m = n;
	else m = NULL;
	return m;
}