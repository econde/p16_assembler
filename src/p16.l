/*
Copyright 2018 Ezequiel Conde

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/* -----------------------------------------------------------------------------
	zona de código de utilizador
*/
%{
#include <string.h>
#include "p16.h"
#include "p16_parser.hpp"

/* handle locations */
int yycolumn = 0;

#define YY_USER_ACTION yylloc.filename = yyfilename; \
    yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}

/* -----------------------------------------------------------------------------
	zona de opções e declarações
*/

%option nodefault
%option noyywrap
%option yylineno
%option nounput

%x STRING
%x CHARLITERAL
%x COMMENT

LineTerminator	\r|\n|\r\n

	/* integer literals */
DecIntegerLiteral	0|[1-9][0-9]*

HexIntegerLiteral	0[xX]0*{HexDigit}{1,8}
HexDigit			[0-9a-fA-F]

OctIntegerLiteral	0+[1-3]?{OctDigit}{1,15}
OctDigit			[0-7]

BinIntegerLiteral	0[bB]0*[01]{1,32}
BinDigit			[01]

/* -----------------------------------------------------------------------------
	zona de regras
"."+(?i:stack)		{ return STACK; }
*/
%%
"."+(?i:section)	{ return SECTION; }
"."+(?i:text)		{ return TEXT; }
"."+(?i:rodata)		{ return RODATA; }
"."+(?i:data)		{ return DATA; }
"."+(?i:bss)		{ return BSS; }
"."+(?i:stack)		{ return STACK; }
"."+(?i:equ)		{ return EQU; }
"."+(?i:align)		{ return ALIGN; }
"."+(?i:byte)		{ yylval.integer = 1; return BYTE; }
"."+(?i:word)		{ yylval.integer = 2; return BYTE; }
"."+(?i:space)		{ return SPACE; }
"."+(?i:ascii)		{ yylval.integer = 0; return ASCII; }
"."+(?i:asciz)		{ yylval.integer = 1; return ASCII; }
	/* operators */
"="					{ return EQUAL; }
"=="				{ return EQUAL_EQUAL; }
"!="				{ return NOT_EQUAL; }
"<"					{ return LESSER; }
"<="				{ return LESSER_EQUAL; }
">"					{ return GREATER; }
">="				{ return GREATER_EQUAL; }
">>"				{ return SHIFT_RIGHT; }
"<<"				{ return SHIFT_LEFT; }
":"					{ return COLON; }
";"					{ return SEMICOLON; }
","					{ return COMMA; }
"&"					{ return AMPERSAND; }
"|"					{ return PIPE; }
"^"					{ return CIRCUMFLEX; }
"~"					{ return TILDE; }
"&&"				{ return AMPERSAND_AMPERSAND; }
"||"				{ return PIPE_PIPE; }
"!"					{ return EXCLAMATION; }
"?"					{ return QUESTION; }
"("					{ return PARENTHESE_LEFT; }
")"					{ return PARENTHESE_RIGHT; }
"["					{ return BRACKET_LEFT; }
"]"					{ return BRACKET_RIGHT; }
"{"					{ return BRACE_LEFT; }
"}"					{ return BRACE_RIGHT; }
"#"					{ return CARDINAL; }
"*"					{ return ASTERISK; }
"/"					{ return SLASH; }
"%"					{ return PERCENT; }
"+"					{ return PLUS; }
"-"					{ return MINUS; }
"."					{ yylval.string = new std::string(string_printf("line#%d", yylineno)); return IDENTIFIER; }

	/* register */

"r0"|"R0"				{ yylval.integer = 0; return REGISTER; }
"r1"|"R1"				{ yylval.integer = 1; return REGISTER; }
"r2"|"R2"				{ yylval.integer = 2; return REGISTER; }
"r3"|"R3"				{ yylval.integer = 3; return REGISTER; }
"r4"|"R4"				{ yylval.integer = 4; return REGISTER; }
"r5"|"R5"				{ yylval.integer = 5; return REGISTER; }
"r6"|"R6"				{ yylval.integer = 6; return REGISTER; }
"r7"|"R7"				{ yylval.integer = 7; return REGISTER; }
"r8"|"R8"				{ yylval.integer = 8; return REGISTER; }
"r9"|"R9"				{ yylval.integer = 9; return REGISTER; }
"r10"|"R10"				{ yylval.integer = 10; return REGISTER; }
"r11"|"R11"				{ yylval.integer = 11; return REGISTER; }
"r12"|"R12"				{ yylval.integer = 12; return REGISTER; }
"r13"|"R13"|(?i:sp)		{ yylval.integer = 13; return REGISTER; }
"r14"|"R14"|(?i:lr)		{ yylval.integer = 14; return REGISTER; }
"r15"|"R15"|(?i:pc)		{ yylval.integer = 15; return REGISTER; }
(?i:cpsr)				{ yylval.integer = CPSR; return SPECIAL; }
(?i:spsr)				{ yylval.integer = SPSR; return SPECIAL; }


	/* instructions */

(?i:ldr)				{ yylval.integer = 0; return LDR_STR; }
(?i:ldrb)				{ yylval.integer = 1; return LDR_STR; }
(?i:str)				{ yylval.integer = 2; return LDR_STR; }
(?i:strb)				{ yylval.integer = 3; return LDR_STR; }

(?i:push)				{ yylval.integer = PUSH; return PUSH_POP; }
(?i:pop)				{ yylval.integer = POP;  return PUSH_POP; }

(?i:add)|(?i:adds)		{ yylval.integer = ADD; return ARITH; }
(?i:adc)|(?i:adcs)		{ yylval.integer = ADC; return ARITH; }
(?i:sub)|(?i:subs)		{ yylval.integer = SUB; return ARITH; }
(?i:sbc)|(?i:sbcs)		{ yylval.integer = SBC; return ARITH; }

(?i:cmp)				{ yylval.integer = CMP; return CMP; }

(?i:and)|(?i:ands)		{ yylval.integer = AND; return LOGIC; }
(?i:orr)|(?i:orrs)		{ yylval.integer = OR; return LOGIC; }
(?i:eor)|(?i:eors)		{ yylval.integer = EOR; return LOGIC; }
(?i:mvn)|(?i:mvns)		{                       return NOT; }

(?i:lsl)|(?i:lsls)		{ yylval.integer = LSL; return SHIFT; }
(?i:lsr)|(?i:lsrs)		{ yylval.integer = LSR; return SHIFT; }
(?i:asr)|(?i:asrs)		{ yylval.integer = ASR; return SHIFT; }
(?i:ror)|(?i:rors)		{ yylval.integer = ROR; return SHIFT; }
(?i:rrx)|(?i:rrxs)		{                       return RRX; }

(?i:movs)				{ return MOVS; }
(?i:mov)				{ yylval.integer = MOV_LOW; return MOV; }
(?i:movt)				{ yylval.integer = MOV_HIGH; return MOVT; }
(?i:msr)				{ return MSR; }
(?i:mrs)				{ return MRS; }

(?i:beq)|(?i:bzs)		{ yylval.integer = EQ; return BRANCH; }
(?i:bne)|(?i:bzc)		{ yylval.integer = NE; return BRANCH; }
(?i:bcs)				{ yylval.integer = CS; return BRANCH; }
(?i:blo)				{ yylval.integer = LO; return BRANCH; }
(?i:bcc)				{ yylval.integer = CC; return BRANCH; }
(?i:bhs)				{ yylval.integer = HS; return BRANCH; }
(?i:blt)				{ yylval.integer = LT; return BRANCH; }
(?i:bge)				{ yylval.integer = GE; return BRANCH; }
(?i:b)					{ yylval.integer = B;  return BRANCH; }
(?i:bl)					{ yylval.integer = BL; return BRANCH; }

	/* numeric literals */

{DecIntegerLiteral}	{ yylval.integer = strtoul(yytext, NULL, 10); return INTEGER_LITERAL; }
{HexIntegerLiteral}	{ yylval.integer = strtoul(yytext, NULL, 16); return INTEGER_LITERAL; }
{OctIntegerLiteral}	{ yylval.integer = strtoul(yytext, NULL, 8);  return INTEGER_LITERAL; }
0[bB]0*[01]{1,32}	{ yylval.integer = strtoul(yytext + 2, NULL, 2);  return INTEGER_LITERAL; }

	/* identifiers */

[.a-zA-Z_][.a-zA-Z0-9_]*	{ yylval.string = new std::string(yytext); return IDENTIFIER; }

	/* label */

[a-zA-Z0-9_]+":"	{ yylval.string = new std::string(yytext); return LABEL; }

{LineTerminator}	{ yycolumn = 0; return EOL; }

	/* ------------------------ comments -------------------------------------*/

"/*"				{ BEGIN COMMENT; }
<COMMENT>"*/"		{ BEGIN INITIAL; }
<COMMENT>([^*]|\n)+|.
<COMMENT><<EOF>>	{ yyerror("Unterminated comment\n"); }

"//".*\n			{ yycolumn = 0; return EOL; }
";".*\n				{ yycolumn = 0; return EOL; }

	/* ----------------------- ignore ----------------------------------------*/

[ \t\f]				{ /* ignore white space */ }
".syntax".*\n
".cpu".*\n
".thumb".*\n
".global".*\n

	/* -------------------- string literal -----------------------------------*/

\"							{ BEGIN STRING; yylval.string = new std::string(); }
<STRING>\"					{ BEGIN INITIAL; yylval.string->append(yytext, strlen(yytext) - 1); return STRING_LITERAL; }
<STRING>[^\n\r\"\\]+		{ yylval.string->append(yytext); }
<STRING>\\t					{ yylval.string->push_back('\t'); }
<STRING>\\n					{ yylval.string->push_back('\n'); }
<STRING>\\r					{ yylval.string->push_back('\r'); }
<STRING>\\\"				{ yylval.string->push_back('\"'); }
<STRING>\\					{ yylval.string->push_back('\\'); }

<STRING>{LineTerminator}	{ yyerror("Unterminated string at end of line"); }


	/* ------------------------ character literal ----------------------------*/

\'										{ BEGIN CHARLITERAL; }
<CHARLITERAL>[^\n\r\'\\]?\'				{ BEGIN INITIAL; yylval.integer = yytext[0]; return CHARACTER_LITERAL; }

	/* escape sequences */
<CHARLITERAL>\\b\'						{ BEGIN INITIAL; yylval.integer = '\b'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\t\'						{ BEGIN INITIAL; yylval.integer = '\t'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\n\'						{ BEGIN INITIAL; yylval.integer = '\n'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\f\'						{ BEGIN INITIAL; yylval.integer = '\f'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\r\'						{ BEGIN INITIAL; yylval.integer = '\r'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\\"\'						{ BEGIN INITIAL; yylval.integer = '\"'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\'\'						{ BEGIN INITIAL; yylval.integer = '\''; return CHARACTER_LITERAL; }
<CHARLITERAL>\\\\\'						{ BEGIN INITIAL; yylval.integer = '\\'; return CHARACTER_LITERAL; }
<CHARLITERAL>\\[0-3]?{OctDigit}?{OctDigit}\' { BEGIN INITIAL; yylval.integer = strtoul(yytext + 1, NULL, 8); return CHARACTER_LITERAL; }

<CHARLITERAL>{LineTerminator}			{ yyerror("Unterminated character literal at end of line"); }
<CHARLITERAL>.							{ yyerror("Illegal character <%s>", yytext); }

.|{LineTerminator}						{ yyerror("Illegal character <%s>", yytext); }

%%
