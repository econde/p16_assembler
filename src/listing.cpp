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

#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include "statement.h"

using namespace ast;
using namespace std;

vector<string>	srcfile;

void listing_load_inputfile(const char *src_filename) {
	try {
		std::ifstream src (src_filename);
		while ( ! src.eof()) {
            std::string line;
			getline(src, line);
			srcfile.push_back(line);
		}
		src.close();
	} catch (std::ios_base::failure& e) {
		std::cerr << "IO exception: " << e.what() << endl;
	}
}

void listing(const char *lst_filename, std::list<Statement*> *ast_root) {
    try {
        std::ofstream lst (lst_filename);
        int current_line = 1;
        std::time_t now = std::time(0);
        lst << "P16 assembler v0.0" << "(" __DATE__ ")\t" << lst_filename << '\t' << ctime(&now) << endl;

        Sections::listing(lst);
        Symbols::listing(lst);

	lst << "Code listing" << endl;
        for (auto s: *ast_root) {
            Statement *statement = s;
            std::string line;
            for (; current_line < statement->location.line; ++current_line) {
			//	linhas em branco e comentários 
                line = srcfile[current_line - 1];
                ostream_printf(lst, "%4d%14c\t", current_line, ' ') << line << endl;
            }
            line = srcfile[current_line - 1];
            lst << statement->listing() << line << endl;
            lst << statement->more_listing();
            current_line++;
        }
        lst.close();
    } catch (std::ios_base::failure& e) {
        std::cerr << "IO exception: " << e.what() << endl;
    }
}
