C90 to MIPS 1 Compiler
==============================

For full project requirements and specification please refer to: https://github.com/LangProc/langproc-2018-cw

-This compiler is written in C++ for ANSI C90
-Lexical analysis is done using Flex, AST tree is parsed using Bison
-Project also includes a (simple) C-to-Python translator
-The target environment is Ubuntu 16 (college lab environment)

Assumptions: input C is pre-processed C90 for both compiler and translator.

Compiler build and execution
----------------------------

The program is built using the command:

    make bin/c_compiler

The compilation function is invoked using the flag `-S`, with
the source file and output file specified on the command line:

    bin/c_compiler -S <source-file.c> -o <dest-file.s>


Translator build and execution
------------------------------

The program is built using the command:

    make bin/c_compiler

The translator function is invoked using the flag `--translate`, with
the source file and output file specified on the command line:

    bin/c_compiler --translate [source-file.c] -o [dest-file.py]

You can assume that the command-line arguments will always be in this
order, and that there will be no spaces in source or destination paths.

Acknowledgements
---------------------------
The project was done in collaboration with Tharusha Amarasinghe and supervised by Dr John Wickerson.
