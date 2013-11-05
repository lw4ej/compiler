#include "symboltable.h"
#include "ast_decl.h"
#include <stdio.h>
void SymbolTable::EnterScope() {

	Assert(Scopes!=NULL&&Scopes->getList()!=NULL);

	Scopes->getList()->Append(this);
}

void SymbolTable::ExitScope() {
	
	Assert(Scopes!=NULL&&Scopes->getList()!=NULL);

	Scopes->getList()->RemoveTail();
}

void SymbolTable::Insert(Decl* d){

	Assert(table!=NULL);
	
	table->Enter(d->getkey(), d);
}

void SymbolTable::Remove(Decl*d){
	
	Assert(table!=NULL);
	
	table->Remove(d->getkey(),d);
}

Decl* SymbolTable::LookupLocal(const char* key){

	Assert(table!=NULL);
	
	return (table->Lookup(key));
}

Decl* SymbolTable::Lookup(const char* key){

	Decl* d = NULL;
	Assert(Scopes!=NULL);
	for(int i= Scopes->getList()->NumElements(); i>0; i--)
		if( (d = Scopes->getList()->Nth(i-1)->LookupLocal(key))!=NULL)
			return d;
	return d;
}
void SymbolTable::appendTable(SymbolTable* ex)
{
	ex->getTable()->Insert(ex->getTable());
}
