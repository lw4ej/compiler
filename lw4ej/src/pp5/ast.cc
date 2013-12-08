/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf
#include "symboltable.h"

Node::Node(yyltype loc) {
    location  = new yyltype(loc);
	parent 	  = NULL;
	nodeTable = NULL;
	codegen   = NULL;
}

Node::Node() {
    location = NULL;
    parent   = NULL;
	nodeTable= NULL;
	codegen  = NULL;
}

Decl* Node::Lookup(Identifier * id, bool local){
	Decl* find=NULL;

	if(!nodeTable)
		ConsTable();

	if(nodeTable != NULL){
		find = nodeTable->Lookup(id->getname());
		if(find)
			return find;
	}

	if( parent && !local)
		return parent->Lookup(id, local);

	return find;
}

Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 
