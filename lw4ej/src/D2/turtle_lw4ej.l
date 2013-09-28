
%{
#include "symtab.h"
#include "token.h"
%}

%%

\/\/.*          ;
[[:space:]]     ;

GO              return GO;
TURN            return TURN; 
VAR             return VAR;
JUMP            return JUMP;
FOR             return FOR;
STEP            return STEP;
TO              return TO;
DO              return DO;
BEGIN           return COPEN;
END             return CCLOSE;


IF              return IF;
ELSE            return ELSE;
THEN            return THEN;
WHILE           return WHILE;
TRUE            return TRUE;
FALSE           return FALSE;
CONTINUE        return CONTINUE;
BREAK           return BREAK;
PROCEDURE       return PROCEDURE;
RETURN          return RETURN;

!               return NOT;
\>              return GT;
\<              return LT;
\>=             return GTE;
\<=             return LTE;
==              return EQ;
!=              return NEQ;
,               return COMMA;
\{              return COPEN;
\}              return CCLOSE;

NORTH           return NORTH;
SOUTH           return SOUTH;
EAST            return EAST;
WEST            return WEST;
UP              return UP;
DOWN            return DOWN;


sin             return SIN;
cos             return COS;
sqrt            return SQRT;

;               return SEMICOLON;
\+              return PLUS;
-               return MINUS;
\*              return TIMES;
\/              return DIV;
\(              return OPEN;
\)              return CLOSE;
=               return ASSIGN;

[a-z]+          { yylval.n = lookup(yytext); 
                  if (yylval.n==NULL)
		    yylval.n = insert(yytext);
                  return ID;
		}
    
[0-9]+\.[0-9]+  yylval.d = atof(yytext);  return FLOAT;
[0-9]+          yylval.i = atoi(yytext);  return NUMBER;


.               fprintf(stderr,"Unexpected character %c (%2X)\n",yytext[0],yytext[0]);

%%
