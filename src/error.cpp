#include <iostream>
#include <vector>
#include <cstdarg>
#include "error.h"
#include "iarm.h"
#include "iarm.tab.hpp"

extern std::vector<std::string>	srcfile;
//extern int yylineno;
//extern int yycolumn;
//extern char *yyfilename;
extern const char *yytext;

using namespace std;

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
	const char *text = srcfile.at(location->line - 1).c_str();
	auto offset = fprintf(stderr, "\n%s (%d): ", location->unit, location->line) - 1;
	fprintf(stderr, "%s\n", text);
	underline(offset, location->first_column, location->last_column, text);
	fprintf(stderr, "\nERROR!\t");

	std::cerr << message << std::endl;
}

void warning_report(ast::Location *location, std::string message) {
    const char *text = srcfile.at(location->line - 1).c_str();
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
	const char *text = srcfile.at(yylloc.first_line - 1).c_str();
	auto offset = fprintf(stderr, "%s (%d): ", yyfilename, yylloc.first_line);
	fprintf(stderr, "%s\n", text);
	underline(offset, yylloc.first_column, yylloc.last_column, text);
    fprintf(stderr, "\nERROR!\t");

    va_list ap;
    va_start(ap, s);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

	error_count++;
}
