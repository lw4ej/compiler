#include "symboltable.h"
#include "ast_decl.h"
#include "errors.h"
#include <stdio.h>

void SymbolTable::Insert(Decl* d){

	Assert(d!=NULL&&table!=NULL);
	Decl* t = table->Lookup(d->getkey());	
	FnDecl *ford = dynamic_cast<FnDecl*>(d);
	FnDecl *fort = dynamic_cast<FnDecl*>(t);
	
	if(t!=NULL){
		if(ford && fort && t->GetParent()!=d->GetParent())
		{
			if(!(ford->CompareFnDecls(fort))){
				ReportError::OverrideMismatch(ford);
				return;
			}
		}
		else {
		
			ReportError::DeclConflict(d ,t);	
			return;
		}
	}
	table->Enter(d->getkey(), d, true);
}

void SymbolTable::Remove(Decl*d){
	
	Assert(d!=NULL&&table!=NULL);
	
	table->Remove(d->getkey(),d);
}

Decl* SymbolTable::Lookup(const char* key){

	Assert(key!=NULL&&table!=NULL);
	
	return (table->Lookup(key));
}

void SymbolTable::appendTable(SymbolTable* ex)
{
	Assert(ex!=NULL);
//	ex->getTable()->Insert(ex->getTable());
	Iterator<Decl*> iter = ex->getTable()->GetIterator();
	Decl * decl;
	while((decl = iter.GetNextValue())!=NULL){
		table->Enter(decl->getkey(), decl, true);
	}
}

void SymbolTable::DisplayTable()
{
	Iterator<Decl*> iter = table->GetIterator();
	Decl *decl;
	while ((decl = iter.GetNextValue()) != NULL) {
		printf("%s\n", decl->getkey());
	}
}
