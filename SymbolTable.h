#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class SymbolTable {
public:
    SymbolTable() {}
    void run(string filename);
};

struct symbol {
	string	name;
	string	type;
	string	value;
	int		level;
	symbol	*left;
	symbol 	*right;
	symbol 	*parent;
	symbol() : left(NULL), right(NULL)	{}
	symbol(string s, string t, int & i) : name(s), type(t), level(i), left(NULL), right(NULL), parent(NULL) {}
};


class symbolTree {
public:
	symbol *root;
public:
	symbolTree() : root(NULL) {}
	
	void left_rotate(symbolTree *t, symbol *x);
	
	void right_rotate(symbolTree *t, symbol *x);
	
	void splay(symbolTree *t, symbol *n, int &num_splay);
	
	int insert(symbolTree *t, symbol *n, int &num_comp, int &num_splay);
	
	int insert_name(symbolTree *t, symbol *n, symbol *y);
	
	void preorder(symbolTree *t, symbol *n);
	
	int lookup(symbolTree *t, symbol *n, string name, int &num_splay, int &num_comp, int level);
	
	void delete_symbol(symbolTree *t, symbol *n, int &num_splay);
	
	bool isNumber(const string& str);
	
	int isString(string str);
	
	int is_Valid_name(string name);
	
	symbol *find_name(symbolTree *t, symbol *n, string name, int level, int &num_comp, int &num_splay);
	
	int is_Valid_type(symbolTree *t, symbol *m, symbol *n, string value, int level, int &num_comp, int &num_splay);
	
	int isOtherSymbol(string str);
	
	int assign(symbolTree *t, symbol *n, string name, string value, int &num_comp, int &num_splay, int level);
	
	int assign_func(symbolTree *t, symbol *n, string name, string value, string f_type);
	
	symbol *maximum(symbolTree *t, symbol *x);
	
	symbol *findparentname(symbolTree *t, symbol *n);
	
	symbol *new_node(string name, string type, int level);
	
	void inorder(symbolTree *t, symbol *n);
	
	symbol *find_to_delete(symbolTree *t, symbol *n, int level);
	
	
};

#endif