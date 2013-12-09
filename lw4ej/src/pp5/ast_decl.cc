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
	offset = 0;
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
	MemAddr = NULL;	
}

void VarDecl::Check() {
	Assert(parent!=NULL && type!=NULL);	
	type->Check();
}

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) {
		extends->SetParent(this);
	}
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
	self = new NamedType(n);
	self -> SetParent(this);
	CompList = new List<NamedType*>;
	methodLabels = new List<const char*>;
}

int ClassDecl::DeployMems(){
	int fnEnum = 0;
	int varEnum = 0;
	
	if(extends != NULL){
		ClassDecl *ext = dynamic_cast<ClassDecl*>(parent->Lookup(extends->getid(),false));
		List<const char*> * labels = ext->getMethodLabels(); 
		varEnum = ext->getsize()-1;
		fnEnum = labels->NumElements();
		for(int i=0; i< fnEnum;i++)
			methodLabels->Append(labels->Nth(i));
	}
	
	for(int i = 0; i< members->NumElements(); i++){
		if(dynamic_cast<VarDecl*>(members->Nth(i))){
			members->Nth(i)->SetOffset(varEnum+1);
			varEnum++;
		}
		else if(dynamic_cast<FnDecl*>(members->Nth(i))){
			members->Nth(i)->SetOffset(fnEnum);
			fnEnum++;
		}
	}
	return varEnum + 1;
}

void ClassDecl::Emit(){
	Assert(nodeTable);
	//set offsets for members;
	size = DeployMems();

	FnDecl * fn = NULL; 

	Assert(parent);
	codegen = parent->GetGenerator();

	for(int i = 0; i< members->NumElements(); i++)
		if((fn = dynamic_cast<FnDecl*>(members->Nth(i)))!=NULL){
			fn->Emit();
			methodLabels->Append(fn->GetLabel());
	}
	codegen->GenVTable(getkey(),methodLabels);
}

SymbolTable* ClassDecl::ConsTable(){

	if(nodeTable!=NULL) return nodeTable;

	nodeTable = new SymbolTable();

	if(extends){
		CompList->Append(extends);
		ClassDecl *ext = dynamic_cast<ClassDecl*>(parent->Lookup(extends->getid(),false));
		if(ext) {
			nodeTable->appendTable(ext->ConsTable());
		}
		else{
			ReportError::IdentifierNotDeclared(extends->getid(), LookingForClass);
			extends = NULL; 
		}
	}

	for(int i = 0 ; i < implements->NumElements(); i++){
		NamedType * impElm = dynamic_cast<NamedType*>(implements->Nth(i));
		CompList->Append(impElm);
		InterfaceDecl * imp = dynamic_cast<InterfaceDecl*>(parent->Lookup(impElm->getid(),false));
		if(imp) {
			nodeTable->appendTable(imp->ConsTable());
		}
		else { 
			ReportError::IdentifierNotDeclared(implements->Nth(i)->getid(),LookingForInterface);
			implements->RemoveAt(i--);
		}
	}

	for(int i = 0; i< members->NumElements(); i++){
		nodeTable->Insert(members->Nth(i));
	}
	//printf("display table\n");
	//nodeTable->DisplayTable();
	return nodeTable;
}

void ClassDecl::Check() {
	ConsTable();
	for(int i = 0; i< members->NumElements(); i++)
		members->Nth(i)->Check();
	CheckImp();
}

void ClassDecl::CheckImp()
{
	for(int j =0; j < implements->NumElements();j++){
		bool Fullimplement = true;
		NamedType * impElm = dynamic_cast<NamedType*>(implements->Nth(j));
		InterfaceDecl * impelm = dynamic_cast<InterfaceDecl*>(parent->Lookup(impElm->getid(),false));
		Assert(impelm!=NULL);
		List<Decl*> *mems = impelm->GetMembers();
		for(int i=0; i< mems->NumElements(); i++){
			FnDecl *fn = dynamic_cast<FnDecl*> (mems->Nth(i));
			Assert(fn != NULL);
			Decl* b = Lookup(fn->getid(),true);
			Assert(b != NULL);
			if(b->GetParent()!=this)
				Fullimplement = false;
		}
		if(Fullimplement==false)	
			ReportError::InterfaceNotImplemented(this,impElm);
	}
}

InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}

SymbolTable* InterfaceDecl::ConsTable(){
	if(nodeTable) return nodeTable;
	nodeTable = new SymbolTable;

	for(int i =0; i< members->NumElements(); i++)
		nodeTable->Insert( members->Nth(i));
	return nodeTable;
}

void InterfaceDecl::Check()
{
	ConsTable();
	for(int i =0; i< members->NumElements(); i++)
		members->Nth(i)->Check();
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;

	char temp_name[50]="";

	if(strcmp(getkey(), "main")!=0)
		strcpy(temp_name, "_");
	ClassDecl* p = NULL;
	if((p=dynamic_cast<ClassDecl*>(parent))!=NULL)
	{
		strcat(temp_name,p->getkey());
		strcat(temp_name,".");
	}
	strcat(temp_name,getkey());
	SetLabel(temp_name);
}

SymbolTable* FnDecl::ConsTable(){
	if(nodeTable) return nodeTable;
	nodeTable = new SymbolTable;

	for(int i=0; i<formals->NumElements(); i++)
		nodeTable->Insert(formals->Nth(i));

	if(body != NULL){
		Assert((dynamic_cast<StmtBlock*>(body))!=NULL);
		body->ConsTable();
	}
	return nodeTable;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

bool FnDecl::CompareFnDecls( FnDecl* b)
{
	List<VarDecl*> *list = b->GetFormals();
	Type* rtype = b->GetRtype();

	Assert(formals && list);

	if(!(GetRtype()->IsEquivalentTo(rtype))) return false;
	if(formals->NumElements() != list->NumElements())
		return false;
	for(int i =0; i< formals->NumElements(); i++){
		VarDecl *formal = dynamic_cast<VarDecl*>(formals->Nth(i));
		VarDecl *flist = dynamic_cast<VarDecl*>(list->Nth(i));
		if(!formal||!flist)
			return false;
		if((formal->GetType()->IsEquivalentTo(list->Nth(i)->GetType()))==false)
			return false;
	}
	return true;
}

void FnDecl::Check(){
	ConsTable();
	returnType->Check();
	for(int i=0; i < formals->NumElements(); i++)
		formals->Nth(i)->Check();
	if(body!=NULL)
	{
		Assert((dynamic_cast<StmtBlock*>(body))!=NULL);
		body->Check();
	}
}

void FnDecl::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen != NULL);

	BeginFunc *beginfn = NULL;
	codegen->GenLabel(Label);
	beginfn = codegen->GenBeginFunc();
	codegen->ResetLocalsAndTemps();
	for(int i=0; i<formals->NumElements(); i++)
		formals->Nth(i)->SetAddr(codegen->GenLocalVar(formals->Nth(i)->getkey()));
	body->Emit();
	beginfn->SetFrameSize(4*codegen->GetLocalsAndTemps());
	codegen->GenEndFunc();
}
