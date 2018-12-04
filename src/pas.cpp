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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <regex>

#include "p16.h"
#include "p16.tab.hpp"

#include "code_generator.h"
#include "relocations.h"
#include "listing_generator.h"

using namespace std;

extern int yydebug;
extern FILE *yyin;
const char *yyfilename;
extern const char *yytext;

void listing(const char *lst_filename, std::list<Statement*> *ast_root);
void listing_load_inputfile(const char *src_filename);

static void help() {
	cout << "Usage: das <options> \"source filename\"" << endl;
	cout << "options:" << endl;
	cout << "\t--verbose" << endl;
	cout << "\t-h, --help" << endl;
	cout << "\t-v, --version" << endl;
	cout << "\t-i, --input \"source filename\"" << endl;
	cout << "\t-o, --output filename" << endl;
	cout << "\t-s, --section sectionname=address" << endl;
}

static void version() {
	cout << "P16 assembler v0.0 " << "(" __DATE__ << ")" << endl;
	cout << "Ezequiel Conde (ezeq@cc.isel.ipl.pt)" << endl;
}

int main(int argc, char **argv) {
	string input_filename, output_filename;
	Properties<string, unsigned> section_addresses;
	int result = 0;
	int verbose_flag = 0;

	static struct option long_options[] = {
		{"verbose", no_argument, &verbose_flag, 1},
		{"help", no_argument, 0, 'h'},
		{"input", required_argument, 0, 'i'},
		{"output", required_argument, 0, 'o'},
		{"section", required_argument, 0, 's'},
		{0, 0, 0, 0}
	};
	int option_index, error_in_options = 0;
        
	int c;
	while ((c = getopt_long(argc, argv, "hvi:o:s:", long_options, &option_index)) != -1) {
		switch (c) {
		case 0:	//	Opções longas com afetação de flag
			break; 
		case 'h':
			help();
			return 0;
		case 'v':
			version();
			return 0;
		case 'i':
			input_filename = optarg;
			break;
		case 'o':
			output_filename = optarg;
			break;
		case 's': {
			regex re("^[\\s]*([\\._]?[a-zAZ][a-zA-Z0-9]*)[\\s]*=[\\s]*(0[xX][0-9a-fA-F]+)[\\s]*$");
			cmatch cm;
			if (regex_match(optarg, cm, re)) {
				int address;
				cout << "{" << cm[1] << "} {" << cm[2] << "}" << endl;
				sscanf(cm[2].str().c_str(), "%i", &address);
				section_addresses.set_property(cm[1].str(), address);
			}
			else {
				printf("Error in parameter of option -s\n");
				error_in_options = 1;
			}
			break;
		}
		case ':':
			printf("Error in parameter of option -%c\n", optopt);
			error_in_options = 1;
			break;
		case '?':
			error_in_options = 1;
			break;
		}
	}
	
	if (error_in_options)
		return -2;
		
	if (argc > optind)
		input_filename = argv[optind];
	
	if (input_filename.empty()) {
		cerr << "Missing input file" << endl;
		return -2;
	}

	if (output_filename.empty())
		output_filename = input_filename.substr(0, input_filename.find_last_of('.'));

	string lst_filename = output_filename + ".lst";
	string hex_filename = output_filename + ".hex";
	string bin_filename = output_filename + ".bin";
	string sim0_filename = output_filename + ".sim0";
	string sim1_filename = output_filename + ".sim1";

	if (verbose_flag) {
		cout << endl;
		cout << "Source file:    " << input_filename << endl;
		cout << "Listing file:   " << lst_filename << endl;
		cout << "Hex Intel file: " << hex_filename << endl;
		cout << "Binary file:    " << bin_filename << endl;
		cout << "Logisim files:  " << sim0_filename << ", " << sim1_filename << endl;		
	}
	
	remove(lst_filename.c_str());
	remove(hex_filename.c_str());
	remove(bin_filename.c_str());
	remove(sim0_filename.c_str());
	remove(sim1_filename.c_str());

	yyin = stdin;
	if ( ! input_filename.empty()) {
		yyin = fopen(input_filename.c_str(), "r");
		if (yyin == NULL) {
			perror(input_filename.c_str());
			return -1;
		}
	}

	listing_load_inputfile(input_filename.c_str());

	yyfilename = input_filename.c_str() + input_filename.find_last_of('/') + 1;
	
	// yydebug = 1;
	if (verbose_flag)
		cout << endl << "Análise sintática e geração da AST" << endl;
	if (yyparse())
		return -1;

	if ( ! input_filename.empty())
		fclose(yyin);

	if (error_count > 0) {
		result = -2;
		goto exit_error;
	}

	if (verbose_flag)
		Symbols::print(cout);

	if (verbose_flag)
		cout << endl << "Gerar código" << endl;
	{
		Code_generator code_gen;
		for (auto s: *ast_root) {
			s->accept(&code_gen);
		}
	}

	if (error_count > 0) {
		result = -2;
		goto exit_error;
	}

	if (verbose_flag)
		cout << endl << "Localizar secções" << endl;

	Sections::locate(&section_addresses);

	if (error_count > 0) {
		result = -2;
		goto exit_error;
	}

	if (verbose_flag) {
		cout << endl << "Relocalizar símbolos" << endl;
		Relocations::print(cout);
	}
	Relocations::relocate();

	if (error_count > 0) {
		result = -2;
		goto exit_error;
	}

	if (verbose_flag)
		cout << endl << "Gerar listagem" << endl;

	listing(lst_filename.c_str(), ast_root);

	if (verbose_flag)
		cout << endl << "Gerar binários" << endl;
	Sections::binary_hex_intel(hex_filename.c_str());

	Sections::binary_raw(bin_filename.c_str());

	Sections::binary_logisim(sim0_filename.c_str(), 2, 0);

	Sections::binary_logisim(sim1_filename.c_str(), 2, 1);

#if 0
	Listing_generator *listing_gen = new Listing_generator();

	for (auto s: *ast_root)
//		cout << s->to_string() << endl;
		s->accept(listing_gen);

	delete listing_gen;
#endif
	result = warning_count > 0 ? -1 : 0;

	exit_error:

    //	Eliminar objetos statment
	for (auto s: *ast_root)
		delete s;
	delete ast_root;

	Sections::deallocate();
	Relocations::deallocate();
	Symbols::deallocate();

	return result;
}
