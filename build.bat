F:\programs\flex-bison\win_flex.exe lexer.l    
F:\programs\flex-bison\win_bison.exe -d parser.y
gcc lex.yy.c parser.tab.c ast.c main.c -o compiler