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
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
	MemAddr = NULL;	
	offset = 0;
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
	//int fnEnum = 0;
	int varEnum = 0;
	FnDecl * fn = NULL;	
	//generate function label first
	for(int i = 0; i< members->NumElements(); i++){
	    if((fn=dynamic_cast<FnDecl*>(members->Nth(i)))!=NULL){
			char temp_name[50]="_";
			strcat(temp_name,getkey());
			strcat(temp_name,".");
			strcat(temp_name,fn->getkey());
			fn->SetLabel(temp_name);
		}
	}
	
	List<const char*> *plabels = NULL;
	if(extends != NULL){
		ClassDecl *ext = dynamic_cast<ClassDecl*>(parent->Lookup(extends->getid(),false));
		varEnum = (ext->getsize())-4;
		plabels = ext->getMethodLabels(); 
		for(int i=0; i< plabels->NumElements();i++){
			char * ptr = strdup(strstr(plabels->Nth(i), "."));
			ptr = ptr+1;
			int j = 0;
			for( j = 0; j< members->NumElements();j++){
				if((fn=dynamic_cast<FnDecl*>(members->Nth(j)))!=NULL && strcmp(fn->getkey(),ptr)==0){
					methodLabels->Append(fn->GetLabel());
					break;
				}
			}
			if(j == members->NumElements())
				methodLabels->Append(plabels->Nth(i));
		}
	}
	
	for(int i = 0; i< members->NumElements(); i++){
		VarDecl * var = dynamic_cast<VarDecl*>(members->Nth(i));
		if(var!=NULL){
			var->SetOffset(varEnum+4);
			varEnum+=4;
		}
		else if((fn=dynamic_cast<FnDecl*>(members->Nth(i)))!=NULL){
		/*	char temp_name[50]="_";
			strcat(temp_name,getkey());
			strcat(temp_name,".");
			strcat(temp_name,fn->getkey());
			fn->SetLabel(temp_name);
			*/
			bool insert = false;

			for(int j=0; j< methodLabels->NumElements();j++){
				if(strcmp(methodLabels->Nth(j), fn->GetLabel())==0)
					insert = true;
			}

			if(insert == false)
				methodLabels->Append(fn->GetLabel());
		}
	}

	return varEnum+4;
}

void ClassDecl::Emit(){
	Assert(nodeTable);
	FnDecl * fn = NULL; 
	Assert(parent);
	codegen = parent->GetGenerator();

	for(int i = 0; i< members->NumElements(); i++)
		if((fn = dynamic_cast<FnDecl*>(members->Nth(i)))!=NULL){
			fn->Emit();
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
	
	//set offsets for members;
	size = DeployMems();
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

	char temp_name[50]="";

	if(strcmp(getkey(), "main")==0){
		strcat(temp_name,getkey());
		SetLabel(temp_name);
	}
	else if((dynamic_cast<ClassDecl*>(parent))==NULL){
		strcpy(temp_name, "_");
		strcat(temp_name,getkey());
		SetLabel(temp_name);
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
	
	int addthis = 0;
	if(dynamic_cast<ClassDecl*>(parent)!=NULL)
		addthis = 4;

	for(int i=formals->NumElements(); i>0; i--){
		VarDecl * arg = formals->Nth(i-1);
		arg->SetAddr(new Location(fpRelative, i*4+addthis, arg->getkey()));
	}
	//	formals->Nth(i)->SetAddr(codegen->GenLocalVar(formals->Nth(i)->getkey()));
	body->Emit();
	beginfn->SetFrameSize(4*codegen->GetLocalsAndTemps());
	codegen->GenEndFunc();
}

int FnDecl::GetOffset(){
	ClassDecl* cls = dynamic_cast<ClassDecl*>(parent);
	if(cls != NULL){
		List<const char *> * l = cls->getMethodLabels();
		for(int i = 0; i< l->NumElements();i++)
			if(strcmp(l->Nth(i),Label)==0)
				return i*4;
	}
	return 0;
}
