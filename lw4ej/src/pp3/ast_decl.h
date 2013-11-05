/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp3: You will need to extend the Decl classes to implement 
 * semantic processing including detection of declaration conflicts 
 * and managing scoping issues.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"

class Type;
class NamedType;
class Identifier;
class Stmt;
class SymbolTable;

class Decl : public Node 
{
  protected:
    Identifier *id;
  public:
    Decl(Identifier *name);
	Identifier* getid(){return id;}
	char* getkey(){return id->getname();}
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    VarDecl(Identifier *name, Type *type);
	Type* getType(){return type;}
	void Constable(SymbolTable*);
	void Check();
	bool comparetype(Type* a);
	//{ return type->IsEquivalentTo(a);}
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;

  public:
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
	void Constable(SymbolTable*);
	void Check();
	void Check_mem(bool, bool);
	void appendBase(SymbolTable*, ClassDecl*);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
	void Constable(SymbolTable*);
	void Check();
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body; 

  public:
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
	void Constable(SymbolTable*);
	List<VarDecl*> * getformals(){return formals;}
	bool compareforms(List<VarDecl*> *list);
	Type* getreturntype(){return returnType;}
	void Check();
};

#endif
