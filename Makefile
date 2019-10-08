CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include
LFLAGS = -d

all : bin/print_canonical bin/eval_expr bin/translate_python bin/c_compiler bin/mips_maker

src/c_parser.tab.cpp src/c_parser.tab.hpp : src/c_parser.y
	bison -v -d src/c_parser.y -o src/c_parser.tab.cpp

src/c_lexer.yy.cpp : src/c_lexer.flex src/c_parser.tab.hpp
	flex $(LFLAGS) -o src/c_lexer.yy.cpp  src/c_lexer.flex

bin/print_canonical : src/print_canonical.o src/c_parser.tab.o src/c_lexer.yy.o src/c_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/print_canonical $^

bin/eval_expr : src/eval_expr.o src/c_parser.tab.o src/c_lexer.yy.o src/c_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/eval_expr $^

bin/translate_python : src/translate_python.o src/c_parser.tab.o src/c_lexer.yy.o src/c_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/translate_python $^

bin/c_compiler : src/c_compiler.o src/c_parser.tab.o src/c_lexer.yy.o src/c_parser.tab.o
		mkdir -p bin
		g++ $(CPPFLAGS) -o bin/c_compiler $^

bin/mips_maker : src/print_mips.o src/c_parser.tab.o src/c_lexer.yy.o src/c_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/mips_maker $^


clean :
	rm src/*.o
	rm bin/*
	rm src/*.tab.cpp
	rm src/*.yy.cpp
	rm src/*.output
	rm src/*.tab.hpp
