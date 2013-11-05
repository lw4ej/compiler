/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h> // strdup
#include <stdio.h>  // printf

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
	children = new List<Node*>;
	cur = nest =NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
	children = new List<Node*>;
	cur = nest =NULL;
}
	 
void Node::SetChildren(List<Node*>*l){
	for(int i=0;i<l->NumElements();i++)
		SetChild(l->Nth(i));
}

void Node::AstWalk( Node* P, void(* callback )(Node* a)) {   
     (*callback)(P);
     if(P->GetChildren()->NumElements())
     for(int i = 0; i < P->GetChildren()->NumElements(); i++){   
          AstWalk(P->GetChildren()->Nth(i), callback);
     }
     
}

Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
	strcpy(nodeName,"Identifier");
} 
