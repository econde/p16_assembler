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

#undef DEBUG

#include "p16.h"
#include "p16_parser.hpp"

#include "code_generator.h"
#include "relocations.h"
#include "listing_generator.h"
#include "version.h"

using namespace std;

extern int yydebug;
extern FILE *yyin;
const char *yyfilename;
extern const char *yytext;

void listing(const char *lst_filename, std::list<Statement*> *ast_root);
void listing_load_inputfile(const char *src_filename);

 static void help(char *prog_name) {
	ostream_printf(cout, "Usage: %s [options] <source filename>\n"
		"options:\n"
		"\t--verbose\n"
		"\t-h, --help\n"
		"\t-v, --version\n"
		"\t-i, --input <source filename>\n"
		"\t-o, --output <base filename>\n"
		"\t-s, --section <section name>=<address>\n"
		"\t-f, --format hexintel | binary | logisim8 | logisim16\n"
		"\t-a, --addresses <from>-<to>\n"
		"\t-l, --interleave\n",
		prog_name);
}

static void version() {
	cout << "p16as - Assembler para o P16" << endl;
	cout << "Versão: " VERSION " (" __DATE__ ")" << endl;
	cout << "Ezequiel Conde (ezeq@cc.isel.ipl.pt)" << endl;
}
static bool validate_output_format(const char *format) {
	return strcmp(format, "hexintel") == 0
		|| strcmp(format, "binary") == 0
		|| strcmp(format, "logisim8") == 0
		|| strcmp(format, "logisim16") == 0;
}

int main(int argc, char **argv) {
	string input_filename, output_filename;
	Properties<string, unsigned> section_addresses;
	int result = 0;
	static int verbose_flag = 0;
	const char *output_format = "hexintel";
	auto word_size = 1U;
	uint32_t lower_address, higher_address;
	bool option_address = false;

	static struct option long_options[] = {
		{"verbose", no_argument, &verbose_flag, 1},
		{"help", no_argument, 0, 'h'},
		{"input", required_argument, 0, 'i'},
		{"output", required_argument, 0, 'o'},
		{"section", required_argument, 0, 's'},
		{"format", required_argument, 0, 'f'},
		{"addresses", required_argument, 0, 'a'},
		{"interleave", required_argument, 0, 'l'},
		{0, 0, 0, 0}
	};

	auto error_in_options = false;

	int option_index, option_char;
	while ((option_char = getopt_long(argc, argv, "hvli:o:s:f:a:", long_options, &option_index)) != -1) {
		switch (option_char) {
		case 0:	//	Opções longas com afetação de flag
			break;
		case 'h':
			help(argv[0]);
			return 0;
		case 'v':
			version();
			break;
		case 'i':
			input_filename = optarg;
			break;
		case 'l':
			word_size = 2;
			break;
		case 'f':
			output_format = optarg;
			if (!validate_output_format(output_format)) {
				fprintf(stderr, "Format %s not permitted in -%c option\n",
						output_format, option_char);
				error_in_options = true;
			}
			break;
		case 'o':
			output_filename = optarg;
			break;
		case 's': {
			regex re("^[\\s]*([\\._]?[a-zA-Z][.a-zA-Z0-9_]*)[\\s]*=[\\s]*(0[xX][0-9a-fA-F]+)[\\s]*$");
			cmatch cm;
			if (regex_match(optarg, cm, re)) {
#ifdef DEBUG
				cout << "cm[0] = " << cm[0]
					 << ", cm[1] = " << cm[1]
					 << ", cm[2] = " << cm[2] << endl;
#endif
				auto address = stoul(cm[2], nullptr, 16);
				section_addresses.set_property(cm[1].str(), address);
			}
			else {
				fprintf(stderr, "Error in option -s argument\n");
				error_in_options = true;
			}
			break;
		}
		case 'a': {
			regex re("^[\\s]*(0[xX][0-9a-fA-F]+)[\\s]*-[\\s]*(0[xX][0-9a-fA-F]+)[\\s]*$");
			cmatch cm;
			if (regex_match(optarg, cm, re)) {
#ifdef DEBUG
				cout << "cm[0] = " << cm[0]
					 << ", cm[1] = " << cm[1]
					 << ", cm[2] = " << cm[2] << endl;
#endif
				lower_address = stoul(cm[1], nullptr, 16);
				higher_address = stoul(cm[2], nullptr, 16);
				option_address = true;
			}
			else {
				fprintf(stderr, "Error in option -a argument\n");
				error_in_options = true;
			}
			break;
		}
		case ':':
			fprintf(stderr, "Error in option -%c argument\n", optopt);
			error_in_options = true;
			break;
		case '?':
			error_in_options = true;
			break;
		}
	}

	if (error_in_options)
		return -2;

	if (argc > optind)
		input_filename = argv[optind];

	if (input_filename.empty()) {
		cerr << "Missing input file" << endl;
		return 1;
	}

	if (output_filename.empty())
		output_filename = input_filename.substr(0, input_filename.find_last_of('.'));

	string lst_filename = output_filename + ".lst";

	if (verbose_flag) {
		cout << endl;
		cout << "Source file: " << input_filename << endl;
	}

	yyin = stdin;
	if ( ! input_filename.empty()) {
		yyin = fopen(input_filename.c_str(), "r");
		if (yyin == NULL) {
			perror(input_filename.c_str());
			return 1;
		}
	}

	listing_load_inputfile(input_filename.c_str());

	yyfilename = input_filename.c_str() + input_filename.find_last_of('/') + 1;

	Sections::set_section(".text");

	// yydebug = 1;
	if (verbose_flag)
		cout << endl << "Syntatic analisis and AST generation" << endl << endl;

	if (yyparse())
		return 1;

	if ( ! input_filename.empty())
		fclose(yyin);

	if (error_count > 0) {
		result = 1;
		goto exit_error;
	}

#if 0
	if (verbose_flag) {
		Symbols::print(cout);
		cout << endl << "Evaluate symbols" << endl << endl;
	}

	Symbols::evaluate();
#endif

	if (verbose_flag) {
		cout << endl << "Print symbols" << endl << endl;
		Symbols::print(cout);
	}

	if (verbose_flag)
		cout << endl << "Code generation" << endl << endl;
	{
		Code_generator code_gen;
		for (auto s: *ast_root) {
			s->accept(&code_gen);
		}
	}

	if (error_count > 0) {
		result = 1;
		goto exit_error;
	}

	if (verbose_flag)
		cout << endl << "Locate Sections" << endl << endl;
	Sections::locate(&section_addresses);
	if (verbose_flag)
		Sections::listing(cout);

	if (error_count > 0) {
		result = 1;
		goto exit_error;
	}

	if (verbose_flag) {
		cout << endl << "Relocate symbols" << endl << endl;
		Relocations::print(cout);
	}
	Relocations::relocate();

	if (error_count > 0) {
		result = 1;
		goto exit_error;
	}

	Sections::fill_memory_space();

	if (verbose_flag) {
		cout << endl << "Generate listing" << endl;
		cout << "\t filename: " << lst_filename << endl;
	}
	remove(lst_filename.c_str());
	listing(lst_filename.c_str(), ast_root);

	//	Binário executável
	if (!option_address) {
		lower_address = Sections::lower_address();
		higher_address = Sections::higher_address();
	}

	if (verbose_flag) {
		cout << endl << "Binary output"<< endl;
		cout <<	"\tformat: " << output_format << endl;
		cout << "\tword_size: " << word_size << endl;
		ostream_printf(cout, "\tlower address: 0x%04x, higher address: 0x%04x\n",
						lower_address, higher_address);
	}

	if (strcmp(output_format, "binary") == 0) {
		if (word_size == 1) {
			string filename = output_filename + ".bin";
			remove(filename.c_str());
			Sections::binary_raw(filename.c_str(), word_size, 0, lower_address, higher_address);
		}
		else {
			for (auto byte_order = 0U; byte_order < word_size; ++byte_order) {
				string bin_filename = output_filename + string_printf("_%d.bin", byte_order);
				remove(bin_filename.c_str());
				Sections::binary_raw(bin_filename.c_str(), word_size, byte_order, lower_address, higher_address);
			}
		}
	}
	else if (strcmp(output_format, "logisim8") == 0) {
		if (word_size == 1) {
			string filename = output_filename + ".sim";
			remove(filename.c_str());
			Sections::binary_logisim(filename.c_str(), word_size, 0, lower_address, higher_address);
		}
		else {
			for (auto byte_order = 0U; byte_order < word_size; ++byte_order) {
				string filename = output_filename + string_printf("_%d.sim", byte_order);
				remove(filename.c_str());
				Sections::binary_logisim(filename.c_str(), word_size, byte_order, lower_address, higher_address);
			}
		}
	}
	else if (strcmp(output_format, "logisim16") == 0) {
		string filename = output_filename + ".sim";
		remove(filename.c_str());
		Sections::binary_logisim16(filename.c_str(), lower_address, higher_address);
	}
	else {	//	Hexadecimal Intel
		if (word_size == 1) {
			string filename = output_filename + ".hex";
			remove(filename.c_str());
			Sections::binary_hex_intel(filename.c_str(), word_size, 0, lower_address, higher_address);
		}
		else {
			for (auto byte_order = 0U; byte_order < word_size; ++byte_order) {
				string filename = output_filename + string_printf("_%d.hex", byte_order);
				remove(filename.c_str());
				Sections::binary_hex_intel(filename.c_str(), word_size, byte_order, lower_address, higher_address);
			}
		}
	}

#if 0
	Listing_generator *listing_gen = new Listing_generator();

	for (auto s: *ast_root)
//		cout << s->to_string() << endl;
		s->accept(listing_gen);

	delete listing_gen;
#endif
	result = warning_count > 0 ? 2 : 0;

exit_error:

    //	Eliminar AST (lista de objetos statment)
	for (auto s: *ast_root)
		delete s;
	delete ast_root;

	Sections::deallocate();
	Relocations::deallocate();
	Symbols::deallocate();

	return result;
}
