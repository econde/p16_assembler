#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <string>
#include "location.h"

extern int error_count;

extern int warning_count;

void error_report(ast::Location *location, std::string message);
void warning_report(ast::Location *location, std::string message);

void error_report(std::string message);

void yyerror(const char *s, ...);
	
#endif
