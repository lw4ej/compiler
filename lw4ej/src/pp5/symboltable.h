#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "hashtable.h"

class Decl;

class SymbolTable
{
	Hashtable<Decl*> *table;
  public:
    SymbolTable() {table = new Hashtable<Decl*>;}
	void appendTable(SymbolTable* extable);
	Hashtable<Decl*> *getTable(){return table; }
	Decl* LookupLocal( const char*);
	Decl* Lookup( const char* );
	void Insert(Decl*);
    void Remove(Decl*);
	void DisplayTable();
};

#endif
