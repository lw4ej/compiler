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
 *
 * pp5: You will need to extend the Decl classes to implement 
 * code generation for declarations.
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
	int offset;
  public:
    Decl(Identifier *name);
	Identifier* getid(){return id;}
	char* getkey(){return id->getname();}
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
	void SetOffset(int i){ offset = i;}
	int GetOffset(){return offset;}
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
	Location* MemAddr;
    
  public:
    VarDecl(Identifier *name, Type *type);
	Type *GetType(){return type;}
	void Check();
	void SetAddr(Location *l){MemAddr = l;}
	Location *GetAddr(){return MemAddr;}
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;

	NamedType* self;
	List<NamedType*> *CompList;
	List<const char*> *methodLabels;
	int size;

  public:
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
	NamedType* getself(){return self;}
	List<NamedType*> * getCompatList(){return CompList;}
	List<const char*> *getMethodLabels(){return methodLabels;}
	SymbolTable *ConsTable();
	SymbolTable *GetNodeTable(){return nodeTable;}
	void Check();
	void CheckImp();
	void Emit();
	int DeployMems();
	int getsize(){return size;}
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
	List<Decl*> *GetMembers(){return members;}
	SymbolTable *ConsTable();
	void Check();
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body; 
	char *Label;

  public:
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
	List<VarDecl*> * GetFormals(){return formals;}
	Type* GetRtype(){return returnType;}
	SymbolTable *ConsTable();
	void Check();
	bool CompareFnDecls(FnDecl *b);
	void Emit();
	void SetLabel(const char * l){ Label = strdup(l);}
	char *GetLabel(){return Label;}
};

#endif
