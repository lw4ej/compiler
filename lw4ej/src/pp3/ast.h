/* File: ast.h
 * ----------- 
 * This file defines the abstract base class Node and the concrete 
 * Identifier and Error node subclasses that are used through the tree as 
 * leaf nodes. A parse tree is a hierarchical collection of ast nodes (or, 
 * more correctly, of instances of concrete subclassses such as VarDecl,
 * ForStmt, and AssignExpr).
 * 
 * Location: Each node maintains its lexical location (line and columns in 
 * file), that location can be NULL for those nodes that don't care/use 
 * locations. The location is typcially set by the node constructor.  The 
 * location is used to provide the context when reporting semantic errors.
 *
 * Parent: Each node has a pointer to its parent. For a Program node, the 
 * parent is NULL, for all other nodes it is the pointer to the node one level
 * up in the parse tree.  The parent is not set in the constructor (during a 
 * bottom-up parse we don't know the parent at the time of construction) but 
 * instead we wait until assigning the children into the parent node and then 
 * set up links in both directions. The parent link is typically not used 
 * during parsing, but is more important in later phases.
 *
 * Semantic analysis: For pp3 you are adding "Check" behavior to the ast
 * node classes. Your semantic analyzer should do an inorder walk on the
 * parse tree, and when visiting each node, verify the particular
 * semantic rules that apply to that construct.

 */

#ifndef _H_ast
#define _H_ast

#include <stdlib.h>   // for NULL
#include "location.h"
#include <iostream>
#include "list.h"
class SymbolTable;

class Node 
{
  protected:
    yyltype *location;
    Node *parent;
	List <Node*> *children;
	char nodeName[20];
	SymbolTable *cur;
	SymbolTable *nest;

  public:
    Node(yyltype loc);
    Node();
  
    yyltype *GetLocation()    { return location; }
    void SetParent(Node *p)   { parent = p; }
    Node *GetParent()         { return parent; }
	void SetChild(Node *c)    { children->Append(c); }
	void SetChildren(List<Node*>*l);       
	List<Node*> *GetChildren(){ return children;}
	char *GetNameForNode()    { return nodeName;}
	char *getname()           { return NULL;}
	//void SetCurtable( SymbolTable* c)
	//						  { cur = c;}
    //void CreateNT(SymbolTable* n)   
	//                         { nest = n;}
	SymbolTable* getNT()      { return nest;}
	SymbolTable* getCT()      { return cur; }

	virtual void Constable( SymbolTable* p ){ cur =p; nest = NULL;}
	virtual void Check(){}

	void AstWalk( Node* P, void(* callback )(Node* a));
};
   

class Identifier : public Node 
{
  protected:
    char *name;
    
  public:
    Identifier(yyltype loc, const char *name);
	char* getname(){return name;}
    friend std::ostream& operator<<(std::ostream& out, Identifier *id) { return out << id->name; }
};


// This node class is designed to represent a portion of the tree that 
// encountered syntax errors during parsing. The partial completed tree
// is discarded along with the states being popped, and an instance of
// the Error class can stand in as the placeholder in the parse tree
// when your parser can continue after an error.
class Error : public Node
{
  public:
    Error() : Node() {}
};



#endif
