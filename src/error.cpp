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

#include <iostream>
#include <vector>
#include <cstdarg>
#include "error.h"
#include "p16.h"
#include "p16_parser.hpp"

extern std::vector<std::string>	srcfile;
//extern int yylineno;
//extern int yycolumn;
//extern char *yyfilename;
extern const char *yytext;

using namespace std;

extern "C" int yychar;

int error_count;

int warning_count;

static void fputnc(char c, int n, FILE *fd) {
	while (n--)
		fputc(c, stderr);
}

static void underline(int offset, int left, int right, const char *text) {
	auto column = offset;
	while (--offset > 0)
		fputc('-', stderr);
	auto column_ref = column;
	for (auto i = 0; i < left; ++i)
		column += (text[i] == '\t') ? 8 - column % 8 : 1;
	fputnc(' ', column - column_ref + 1, stderr);

	column_ref = column;
	for (auto i = left; i <= right;  ++i)
		column += text[i] == '\t' ? 8 - column % 8 : 1;
	fputnc('^', column - column_ref, stderr);

	// fprintf(stderr, " %d %d %d", offset, left, right);
}

void error_report(ast::Location *location, std::string message) {
	error_count++;
//	int lineno = yylloc.first_line - (yychar != EOL ?  1 : 2);
	int lineno =  location->line - 1;
	const char *text = srcfile.at(lineno).c_str();
	auto offset = fprintf(stderr, "\n%s (%d): ", location->unit, location->line) - 1;
	fprintf(stderr, "%s\n", text);
	underline(offset, location->first_column, location->last_column, text);
	fprintf(stderr, "\nERROR!\t");

	std::cerr << message << std::endl;
}

void warning_report(ast::Location *location, std::string message) {
//	int lineno = yylloc.first_line - (yychar != EOL ?  1 : 2);
	int lineno =  location->line - 1;
	const char *text = srcfile.at(lineno).c_str();
	auto offset = fprintf(stderr, "\n%s (%d): ", location->unit, location->line) - 1;
	fprintf(stderr, "%s\n", text);
	underline(offset, location->first_column, location->last_column, text);
	fprintf(stderr, "\nWARNING!\t");

	std::cerr << message << std::endl;

	warning_count++;
}

void error_report(std::string message) {
	std::cerr << message << std::endl;

	error_count++;
}

void yyerror(const char *s, ...) {
	int lineno = yylloc.first_line - (yychar != EOL ?  1 : 2);
	const char *text = srcfile.at(lineno).c_str();
	auto offset = fprintf(stderr, "%s (%d): ", yyfilename, yylloc.first_line);
	fprintf(stderr, "%s\n", text);
	underline(offset, yylloc.first_column, yylloc.last_column, text);
	fprintf(stderr, "\nERROR!\t");

	va_list ap;
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");

	error_count++;
	exit(-1);
}
