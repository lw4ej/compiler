#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "hashtable.h"
#include "list.h"
//#include "ast_decl.h"

class ActScope;
class Decl;
class SymbolTable
{
	Hashtable<Decl*> *table;
	ActScope* Scopes;
  public:
    SymbolTable() { Scopes = NULL; table = new Hashtable<Decl*>;}
	void appendTable(SymbolTable* extable);
	Hashtable<Decl*> *getTable(){return table; }
	void SetActScope( ActScope* s) { Scopes = s;}
	ActScope* GetActScope() { return Scopes;}
	void EnterScope();
	void ExitScope();
	Decl* LookupLocal( const char*);
	Decl* Lookup( const char* );
	void Insert(Decl*);
    void Remove(Decl*);
};

class ActScope
{
   protected:
   	  List<SymbolTable*> *activeList;
   public:
      ActScope(){ activeList = new List<SymbolTable*>;}
	  List<SymbolTable*> *getList(){return activeList;};
};

#endif
