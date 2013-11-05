/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"        
#include "errors.h"   

Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
	SetChild(n);
	strcpy(nodeName, "Decl");
}

void VarDecl::Constable(SymbolTable* p){ 
	cur = p;
	nest = NULL;
	p->Insert(this); 
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
	SetChild(t);
	strcpy(nodeName, "VarDecl");
}
bool VarDecl:: comparetype(Type* a){ return type->IsEquivalentTo(a);}

void VarDecl::Check() {

	Assert(cur!=NULL);
	Decl* b = cur->LookupLocal(getkey());
	NamedType *t =NULL ;
	Type * tp = NULL;
	ArrayType *m = NULL;
	
	
	if(b!=NULL && (b->GetLocation()->first_line != this->GetLocation()->first_line
		|| b->GetLocation()->first_column != this->GetLocation()->first_column))
		ReportError::DeclConflict(b, this);
	
	if( (t = dynamic_cast<NamedType*>(type))!=NULL ){
		b = cur->Lookup(t->getid()->getname());
		if( b==NULL ||(dynamic_cast<ClassDecl*>(b)==NULL&&dynamic_cast<InterfaceDecl*>(b)==NULL ))
			ReportError::IdentifierNotDeclared(t->getid(),LookingForType);
	}
	else{
		tp = type;	
		while( (m=dynamic_cast<ArrayType*>(tp))!=NULL ){
			tp = m->getEleType();
		}
		if( (t = dynamic_cast<NamedType*>(tp))!=NULL ){
			b = cur->Lookup(t->getid()->getname());
			if( b==NULL ||(dynamic_cast<ClassDecl*>(b)==NULL&&dynamic_cast<InterfaceDecl*>(b)==NULL))
				ReportError::IdentifierNotDeclared(t->getid(),LookingForType);
		}
	}
}

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) {
		extends->SetParent(this);
		SetChild(extends);
	}
    (implements=imp)->SetParentAll(this);
	SetChildren((List<Node*>*)implements);
    (members=m)->SetParentAll(this);
	SetChildren((List<Node*>*)members);
	strcpy(nodeName, "ClassDecl");
}

void ClassDecl::Constable(SymbolTable*p){
	cur = p;
	cur->Insert(this);
	nest = new SymbolTable;
	for(int i =0; i< members->NumElements(); i++)
		members->Nth(i)->Constable(getNT());
}
void ClassDecl::Check() {
	
	Assert(cur!=NULL&& nest!=NULL&& cur->GetActScope()!=NULL);
	Decl* b = cur->LookupLocal(getkey());
	FnDecl* fn = NULL;
	bool HasExtend = false;
	bool HasImp = false;

	if(b!=NULL && (b->GetLocation()->first_line != this->GetLocation()->first_line
		|| b->GetLocation()->first_column != this->GetLocation()->first_column))
		ReportError::DeclConflict(b,this);
	
	if(extends){
	 	b = cur->Lookup(extends->getid()->getname());
		if( b==NULL ||(dynamic_cast<ClassDecl*>(b)==NULL ))
			ReportError::IdentifierNotDeclared(extends->getid(),LookingForClass);
		else HasExtend = true;
	}
	if( implements->NumElements() > 0 )
		HasImp = true;
	for(int i =0; i< implements->NumElements(); i++ ){
	 	if((b = cur->Lookup(implements->Nth(i)->getid()->getname()))==NULL){
			HasImp = false;
			ReportError::IdentifierNotDeclared(implements->Nth(i)->getid(),LookingForInterface);
		}
	}

	Assert(nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	Check_mem(HasExtend,HasImp);
	nest->ExitScope();
}

void ClassDecl::Check_mem( bool HasExtend, bool HasImp)
{
	NamedType *t =NULL;
	ClassDecl *ext = NULL;
	InterfaceDecl *imp = NULL;
	Decl *mem = NULL;
	FnDecl * fn = NULL;
	FnDecl * memi = NULL;
	VarDecl *vari=NULL;

	for(int i =0; i< members->NumElements(); i++){
		
		Assert((members->Nth(i)->getCT())==nest);
		members->Nth(i)->Check();
		memi = NULL;

		if((memi = dynamic_cast<FnDecl*>(members->Nth(i)))!=NULL)
		{
			if( HasExtend ){
				t = NULL;
				ext = NULL;
				mem=NULL;
				fn = NULL;

				Assert((t = dynamic_cast<NamedType*>(extends))!=NULL);
			
				ext = dynamic_cast<ClassDecl*>(cur->Lookup(t->getkey()));
				
				Assert(ext!=NULL && ext->getNT());
				
				mem = ext->getNT()->LookupLocal(memi->getkey());
				fn = dynamic_cast<FnDecl*>(mem);

				if(fn!=NULL){
					if( memi->compareforms(fn->getformals())==false||
					   (memi->getreturntype()->IsEquivalentTo(fn->getreturntype()))== false )
						ReportError::OverrideMismatch(memi);
				}
				

			}
		
			if( HasImp ){	
				for(int j =0; j< implements->NumElements();j++){
					t = NULL;
					imp = NULL;
					mem = NULL;
					fn = NULL;

					Assert((t = dynamic_cast<NamedType*>(implements->Nth(j)))!=NULL);
			
					imp = dynamic_cast<InterfaceDecl*>(cur->Lookup(t->getkey()));

					Assert(imp!=NULL&&imp->getNT());

					mem = imp->getNT()->LookupLocal(memi->getkey());
				    if(mem)	fn = dynamic_cast<FnDecl*>(mem);
					
					if(fn!=NULL){
						//compare fn and members->Nth(i)
						if( memi->compareforms(fn->getformals())==false
						||(memi->getreturntype()->IsEquivalentTo(fn->getreturntype()))== false )
							ReportError::OverrideMismatch(memi);
						break;
					}

				}
			}
		}

		else if(HasExtend){	
			Assert((vari = dynamic_cast<VarDecl*>(members->Nth(i)))!=NULL);
			Assert((t = dynamic_cast<NamedType*>(extends))!=NULL);
			
			ext = dynamic_cast<ClassDecl*>(cur->Lookup(t->getkey()));
			Assert(ext!=NULL && ext->getNT());
			
			mem = NULL;
			mem = ext->getNT()->LookupLocal(vari->getkey());

			if(mem!=NULL){
				ReportError::DeclConflict(vari, mem);
			}

		}
	}
	if(ext) nest->appendTable(ext->cur);
}

InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
	SetChildren((List<Node*>*)members);
	strcpy(nodeName, "InterfaceDecl");
}

void InterfaceDecl::Constable(SymbolTable*p){
	cur = p;
	cur->Insert(this);
	nest = new SymbolTable;
	for(int i =0; i< members->NumElements(); i++)
		members->Nth(i)->Constable(getNT());
}
void InterfaceDecl::Check()
{	
	Assert(cur!=NULL);
	Decl* b = cur->LookupLocal(getkey());

	if(b!=NULL && (b->GetLocation()->first_line != this->GetLocation()->first_line
		|| b->GetLocation()->first_column != this->GetLocation()->first_column))
		ReportError::DeclConflict(b,this);

	Assert(nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	for(int i=0; i<members->NumElements();i++){
		Assert(members->Nth(i)->getCT() == nest);
		members->Nth(i)->Check();
	}
	nest->ExitScope();
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
	SetChild(returnType);
    (formals=d)->SetParentAll(this);
	SetChildren((List<Node*>*)formals);
    body = NULL;
	strcpy(nodeName, "FnDecl");
}

void FnDecl::Constable(SymbolTable*p){
	cur = p;
	cur->Insert(this);
	nest = new SymbolTable;
	Assert(getNT()==nest);
	for(int i=0; i<formals->NumElements(); i++)
		formals->Nth(i)->Constable(nest);
	if(body!=NULL){
		Assert((dynamic_cast<StmtBlock*>(body))!=NULL);
		body->Constable(nest);
	}
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
	SetChild(body);
}

bool FnDecl::compareforms(List<VarDecl*> *list)
{
	Assert(formals && list);

	if(formals->NumElements() != list->NumElements())
		return false;
	for(int i =0; i< formals->NumElements(); i++){
		if((formals->Nth(i)->comparetype(list->Nth(i)->getType()))==false)
			return false;
	}
	return true;
}

void FnDecl::Check(){
	
	Assert(cur!=NULL);
	Decl* b = cur->LookupLocal(getkey());
	NamedType * t = NULL;
	
	if(b!=NULL && (b->GetLocation()->first_line != this->GetLocation()->first_line
		|| b->GetLocation()->first_column != this->GetLocation()->first_column))
		ReportError::DeclConflict(b,this);

	if( (t = dynamic_cast<NamedType*>(returnType))!=NULL ){
		//b = cur->Lookup(t->getid()->getname());
		b=cur->Lookup(t->getkey());
		if( b==NULL ||(dynamic_cast<ClassDecl*>(b)==NULL&& dynamic_cast<InterfaceDecl*>(b)==NULL))
			ReportError::IdentifierNotDeclared(t->getid(),LookingForType);
	}

	Assert(nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	for(int i=0; i<formals->NumElements();i++){
		Assert(formals->Nth(i)->getCT() == nest);
		formals->Nth(i)->Check();
	}
	if(body!=NULL)
	{
		Assert((dynamic_cast<StmtBlock*>(body))!=NULL);
		body->Check();
	}
	nest->ExitScope();
}

