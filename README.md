# mini-language
The goal of this project is to implement a C11 program, named runml, which accepts a single command-line argument providing the pathname of a text file containing a program written in ml. Successful execution of runml will involve checking the syntax of the ml program, translating that valid ml program to a C11 program, compilation of the resultant C program and, finally, execution of the compiled program. 

Compile command: 
```bash
cc -std=c11 -Wall -Werror -o runml runml.c
```

## our _ml_ lanuage
1. programs are written in text files whose names end in .ml
2. statements are written one-per-line (with no terminating semi-colon)
3. the character '#' appearing anywhere on a line introduces a comment which extends until the end of that line 
4.  only a single datatype is supported - real numbers, such as 2.71828
5. identifiers (names) consist of 1..12 lowercase alphabetic characters, such as budgie
6. identifiers do not need to be defined before being used in an expression, and are automatically initialised to the (real) value 0.0
7. the identifiers arg0, arg1, and so on, provide access to the program's command-line arguments which provide real-valued numbers
8. a function must have been defined before it is called in an expression
9. each statement in a function's body (one-per-line) is indented with a tab character
10. functions may have zero-or-more formal parameters
11. a function's parameters and any other identifiers used in a function body are local to that function, and 'disappear' when the function's execution completes 

```txt
The ml syntax — [...] zero or one,   (...)* zero or more

program:
           ( statement )*
        |  function identifier [ identifier ( "," identifier )* ]
           ←–tab–→ statement1
           ←–tab–→ statement2
           ....

statement:
           identifier "<-" expression
        |  print  expression
        |  return expression
        |  functioncall

expression:
          term   [ ("+" | "-")  expression ]

term:
          factor [ ("*" | "/")  term ]

factor:
          realconstant
        | identifier
        | functioncall
        | "(" expression ")"

functioncall:
          identifier "(" [ expression ( "," expression )* ] ")"
```