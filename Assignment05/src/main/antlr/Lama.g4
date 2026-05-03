grammar Lama;

@header {
package io.github.dtolmachev.parser;
}

options {
    tokenVocab = LamaLexer;
}



compilationUnit
    : importStatement* scopeExpression EOF
    ;

importStatement
    : KEYWORD_IMPORT UIDENT DELIMITER_SEMICOLON
    ;



scopeExpression
    : definition* expression?
    ;

definition
    : variableDefinition
    | functionDefinition
    ;

variableDefinition
    : (KEYWORD_VAR | KEYWORD_PUBLIC) variableDefinitionSequence DELIMITER_SEMICOLON
    ;

variableDefinitionSequence
    : variableDefinitionItem (DELIMITER_COMMA variableDefinitionItem)*
    ;

variableDefinitionItem
    : LIDENT (DELIMITER_EQUAL basicExpression)?
    ;

functionDefinition
    : KEYWORD_PUBLIC? KEYWORD_FUN LIDENT DELIMITER_LEFT_PAREN functionArguments DELIMITER_RIGHT_PAREN functionBody
    ;

functionArguments
    : (pattern (DELIMITER_COMMA pattern)*)?
    ;

functionBody
    : DELIMITER_LEFT_BRACE scopeExpression DELIMITER_RIGHT_BRACE
    ;



expression
    : basicExpression (DELIMITER_SEMICOLON basicExpression)*
    ;

basicExpression
    : binaryExpression
    ;

binaryExpression
    : INFIX_SUBTRACTION? postfixExpression # unaryExpression
    | binaryExpression infixMultiplicative binaryExpression # multiplicativeExpression
    | binaryExpression infixAdditive binaryExpression # additiveExpression
    | binaryExpression infixComparison binaryExpression # comparisonExpression
    | binaryExpression INFIX_CONJUNCTION binaryExpression # conjunctionExpression
    | binaryExpression INFIX_DISJUNCTION binaryExpression # disjunctionExpression
    | <assoc=right> binaryExpression INFIX_LIST_CONSTRUCTOR binaryExpression # listConstructorExpression
    | <assoc=right> binaryExpression INFIX_ASSIGNMENT binaryExpression # assignmentExpression
    ;

postfixExpression
    : primary # primaryExpression
    | postfixExpression DELIMITER_LEFT_PAREN (expression (DELIMITER_COMMA expression)*)? DELIMITER_RIGHT_PAREN # callExpression
    | postfixExpression DELIMITER_DOT LIDENT (DELIMITER_LEFT_PAREN (expression (DELIMITER_COMMA expression)*)? DELIMITER_RIGHT_PAREN)? # methodCallExpression
    | postfixExpression DELIMITER_LEFT_BRACKET expression DELIMITER_RIGHT_BRACKET # subscriptExpression
    ;

primary
    : DECIMAL # integerLiteral
    | STRING # stringLiteral
    | CHAR # charLiteral
    | LIDENT # nameReference
    | KEYWORD_TRUE # trueLiteral
    | KEYWORD_FALSE # falseLiteral
    | KEYWORD_FUN DELIMITER_LEFT_PAREN functionArguments DELIMITER_RIGHT_PAREN functionBody # functionPrimary
    | KEYWORD_SKIP # skipPrimary
    | DELIMITER_LEFT_PAREN scopeExpression DELIMITER_RIGHT_PAREN # parenthesizedExpression
    | listExpression # listLiteral
    | arrayExpression # arrayLiteral
    | sExpression # sExpressionLiteral
    | ifExpression # ifPrimary
    | whileDoExpression # whileDoPrimary
    | doWhileExpression # doWhilePrimary
    | forExpression # forPrimary
    | caseExpression # casePrimary
    | letExpression # letPrimary
    ;

listExpression
    : DELIMITER_LEFT_BRACE (expression (DELIMITER_COMMA expression)*)? DELIMITER_RIGHT_BRACE
    ;

arrayExpression
    : DELIMITER_LEFT_BRACKET (expression (DELIMITER_COMMA expression)*)? DELIMITER_RIGHT_BRACKET
    ;

sExpression
    : UIDENT (DELIMITER_LEFT_PAREN expression (DELIMITER_COMMA expression)* DELIMITER_RIGHT_PAREN)?
    ;

ifExpression
    : KEYWORD_IF expression KEYWORD_THEN scopeExpression elsePart? KEYWORD_FI
    ;

elsePart
    : KEYWORD_ELIF expression KEYWORD_THEN scopeExpression elsePart? # elifExpression
    | KEYWORD_ELSE scopeExpression # elseExpression
    ;

whileDoExpression
    : KEYWORD_WHILE expression KEYWORD_DO scopeExpression KEYWORD_OD
    ;

doWhileExpression
    : KEYWORD_DO scopeExpression KEYWORD_WHILE expression KEYWORD_OD
    ;

forExpression
    : KEYWORD_FOR scopeExpression DELIMITER_COMMA expression DELIMITER_COMMA expression KEYWORD_DO scopeExpression KEYWORD_OD
    ;

caseExpression
    : KEYWORD_CASE expression KEYWORD_OF caseBranches KEYWORD_ESAC
    ;

caseBranches
    : caseBranch (DELIMITER_VERTICAL_BAR caseBranch)*
    ;

caseBranch
    : pattern DELIMITER_ARROW scopeExpression
    ;

letExpression
    : KEYWORD_LET pattern DELIMITER_EQUAL expression KEYWORD_IN expression
    ;

pattern
    : consPattern
    | simplePattern
    ;

consPattern
    : simplePattern INFIX_LIST_CONSTRUCTOR pattern
    ;

simplePattern
    : wildcardPattern # simpleWildcardPattern
    | sExpressionPattern # simpleSExpressionPattern
    | arrayPattern # simpleArrayPattern
    | listPattern # simpleListPattern
    | LIDENT (DELIMITER_AT pattern)? # nameReferencePattern
    | INFIX_SUBTRACTION? DECIMAL # integerLiteralPattern
    | STRING # stringLiteralPattern
    | CHAR # charLiteralPattern
    | KEYWORD_TRUE # trueLiteralPattern
    | KEYWORD_FALSE # falseLiteralPattern
    | DELIMITER_HASH KEYWORD_BOX # hashBoxPattern
    | DELIMITER_HASH KEYWORD_VAL # hashValPattern
    | DELIMITER_HASH KEYWORD_STR # hashStrPattern
    | DELIMITER_HASH KEYWORD_ARRAY # hashArrayPattern
    | DELIMITER_HASH KEYWORD_SEXP # hashSexpPattern
    | DELIMITER_HASH KEYWORD_FUN # hashFunPattern
    | DELIMITER_LEFT_PAREN pattern DELIMITER_RIGHT_PAREN # parenthesizedPattern
    ;

wildcardPattern
    : DELIMITER_UNDERSCORE
    ;

sExpressionPattern
    : UIDENT (DELIMITER_LEFT_PAREN pattern (DELIMITER_COMMA pattern)* DELIMITER_RIGHT_PAREN)?
    ;

arrayPattern
    : DELIMITER_LEFT_BRACKET (pattern (DELIMITER_COMMA pattern)*)? DELIMITER_RIGHT_BRACKET
    ;

listPattern
    : DELIMITER_LEFT_BRACE (pattern (DELIMITER_COMMA pattern)*)? DELIMITER_RIGHT_BRACE
    ;



infixComparison
    : INFIX_EQUAL # InfixEqual
    | INFIX_NOT_EQUAL # InfixNotEqual
    | INFIX_LESS_OR_EQUAL # InfixLessOrEqual
    | INFIX_LESS # InfixLess
    | INFIX_GREATER_OR_EQUAL # InfixGreaterOrEqual
    | INFIX_GREATER # InfixGreater
    ;

infixAdditive
    : INFIX_ADDITION # InfixAddition
    | INFIX_SUBTRACTION# InfixSubtraction
    ;

infixMultiplicative
    : INFIX_MULTIPLICATION # InfixMultiplication
    | INFIX_QUOTIENT # InfixQuotient
    | INFIX_REMAINDER # InfixRemainder
    ;
