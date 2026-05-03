lexer grammar LamaLexer;

@header {
package io.github.dtolmachev.parser;
}



DELIMITER_DOT : '.';
DELIMITER_COMMA : ',';
DELIMITER_LEFT_PAREN : '(';
DELIMITER_RIGHT_PAREN : ')';
DELIMITER_LEFT_BRACE : '{';
DELIMITER_RIGHT_BRACE : '}';
DELIMITER_LEFT_BRACKET : '[';
DELIMITER_RIGHT_BRACKET : ']';
DELIMITER_SEMICOLON : ';';
DELIMITER_HASH : '#';
DELIMITER_ARROW : '->';
DELIMITER_VERTICAL_BAR : '|';
DELIMITER_EQUAL : '=';
DELIMITER_AT : '@';
DELIMITER_UNDERSCORE : '_';

INFIX_ASSIGNMENT : ':=';
INFIX_LIST_CONSTRUCTOR : ':';
INFIX_DISJUNCTION : '!!';
INFIX_CONJUNCTION : '&&';
INFIX_EQUAL : '==';
INFIX_NOT_EQUAL : '!=';
INFIX_LESS_OR_EQUAL : '<=';
INFIX_LESS : '<';
INFIX_GREATER_OR_EQUAL : '>=';
INFIX_GREATER : '>';
INFIX_ADDITION : '+';
INFIX_SUBTRACTION : '-';
INFIX_MULTIPLICATION : '*';
INFIX_QUOTIENT : '/';
INFIX_REMAINDER : '%';

KEYWORD_ARRAY : 'array';
KEYWORD_BOX : 'box';
KEYWORD_CASE : 'case';
KEYWORD_DO : 'do';
KEYWORD_ELIF : 'elif';
KEYWORD_ELSE : 'else';
KEYWORD_ESAC : 'esac';
KEYWORD_FALSE : 'false';
KEYWORD_FI : 'fi';
KEYWORD_FOR : 'for';
KEYWORD_FUN : 'fun';
KEYWORD_IF : 'if';
KEYWORD_IMPORT : 'import';
KEYWORD_OD : 'od';
KEYWORD_OF : 'of';
KEYWORD_PUBLIC : 'public';
KEYWORD_SEXP : 'sexp';
KEYWORD_SKIP : 'skip';
KEYWORD_STR : 'str';
KEYWORD_THEN : 'then';
KEYWORD_TRUE : 'true';
KEYWORD_VAL : 'val';
KEYWORD_VAR : 'var';
KEYWORD_WHILE : 'while';
KEYWORD_LET : 'let';
KEYWORD_IN : 'in';

UIDENT : [A-Z] [a-zA-Z_0-9]*;
LIDENT : [a-z] [a-zA-Z_0-9]*;
DECIMAL : [0-9]+;
STRING : '"' (~["\r\n] | '""')* '"';
CHAR : '\'' (~['\r\n] | '\'\'' | '\\n' | '\\t') '\'';

WHITESPACE : [ \n\r\t]+ -> skip;
END_OF_LINE_COMMENT : '--' ~[\r\n]* -> skip;
BLOCK_COMMENT : '(*' .*? '*)' -> skip;
