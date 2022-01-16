# Symbol-Table-Using-SplayTree_Structure
## Introduction
Symbol table is a crucial data structure, made and maintained by compilers to trace semantics
of identifiers (e.g information about name, type, scope, e.t.c).

In the previous assignment, students were required to implement simulations of symbol
table via list data structure. However, indexing speed for checking purpose in this kind of data
structure is not efficient. Whenever the source program has plenty of variables, saved in various
scopes, it will become ineffective. On the other hand, in practice, programmers often tend to
use newly declared or recently used identifiers for the next command lines, making the process
of indexing those identifiers becomes more popular.

In this assignment, students are required to implement a simulation of a symbol table,
using splay tree data structures to facilitate those disadvantages mentioned above.
