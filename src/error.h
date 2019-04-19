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
