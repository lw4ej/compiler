
%{
#include "symtab.h"
#include "turtle-lw4ej.tab.h"
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

IF              return IF;              //Conditional
ELSE            return ELSE;            //Conditional   

==              return EQ;              //conditional
!=              return NEQ;             //conditional
\>              return GT;              //conditional
\>=             return GTE;             //conditional
\<              return LT;              //conditional
\<=             return LTE;             //conditional

\{              return COPEN;           //copen
\}              return CCLOSE;          //cclose

WHILE           return WHILE;           //while-loop

PROCEDURE       return PROCEDURE;       //procedure
CALL            return CALL;            //procedure
PARAM           return PARAM;           //procedure

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

