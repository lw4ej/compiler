
all: turtle result

result: eins.tlt if_test-syn while_test-syn proc_test-syn turtle
	./turtle < eins.tlt > eins.ps
	./turtle < if_test-syn > if_test-syn.ps
	./turtle < while_test-syn > while_test-syn.ps
	./turtle < proc_test-syn > proc_test-syn.ps

lex.yy.o:  lex.yy.c turtle-lw4ej.tab.h symtab.h
	gcc -c lex.yy.c

lex.yy.c: turtle-lw4ej.l
	flex turtle-lw4ej.l
	
turtle-lw4ej.tab.h: turtle-lw4ej.y
	bison -d turtle-lw4ej.y
	
turtle-lw4ej.tab.c: turtle-lw4ej.y
	bison -d turtle-lw4ej.y
	
turtle-lw4ej.tab.o: turtle-lw4ej.tab.c symtab.h
	gcc -c turtle-lw4ej.tab.c

turtle: turtle-lw4ej.tab.o lex.yy.o symtab.c
	gcc lex.yy.o turtle-lw4ej.tab.o symtab.c -lfl -o turtle

clean:
	rm -f turtle lex.* turtle-lw4ej.tab.* *.o *.output

