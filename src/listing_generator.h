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

#include "visitor.h"

using namespace ast;


class Listing_generator: public Visitor {

	static int line;

	//	Statement

	void visit(Statement_empty *s) {
		cout << s->location.line << "\t" << s->to_string() << endl;
	}

	//------------------------------------------------------------------------------------------
	//	Instruction

	void visit(Load_relative *s) {
		cout << s->location.line << '\t' << s->Statement::to_string() << 
			std::string((s->ldst & 2) ? "str" : "ldr") << ((s->ldst & 1) ? 'b' : ' ') <<
				"\t" << register_name(s->rd->n) << ", ";
		s->constant->accept(this);
		cout << endl;
	}

	void visit(Load_store_indirect *s) {
	cout << s->location.line << '\t' << s->Statement::to_string()
			 << std::string((s->ldst & 2) ? "str" : "ldr") << ((s->ldst & 1) ? 'b' : ' ')
			 <<	"\t" << register_name(s->rd->n) << ", [" << register_name(s->rn->n) << ", ";
		if (s->constant == nullptr)
			cout << register_name(s->rm->n);
		else
			s->constant->accept(this);
		cout << "]" << endl;
    } 

	void visit(Branch *s) {
    	cout << s->location.line << "\t" << s->Statement::to_string()
			 <<	branch_name(s->condition) << "\t";
			s->expression->accept(this);
		cout << endl;
    }

    void visit(Arith *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
			 << arith_name(s->operation) << "\t" << register_name(s->rd->n)
			 << ", " << register_name(s->rn->n) << ", ";
    	if (s->expression == nullptr)
			cout << register_name(s->rm->n);
		else
			s->expression->accept(this);
		cout << endl;
    }

    void visit(Compare *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
			 << "cmp\t" + register_name(s->rn->n) << ", ";
    	if (s->constant == nullptr)
			cout << register_name(s->rm->n);
		else
			s->constant->accept(this);
		cout << endl;
    }

    void visit(Logic *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
			 << logic_name(s->operation) << "\t" << register_name(s->rd->n) << ", "
             << register_name(s->rn->n) << ", " << register_name(s->rm->n) << endl;
    }

    void visit(Not *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << "not\t" << register_name(s->rd->n) << ", " << register_name(s->rn->n) << endl;
    }

    void visit(Shift *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << shift_name(s->operation) << "\t" << register_name(s->rd->n) << ", "
             << register_name(s->rn->n) << ", ";
		s->constant->accept(this);
		cout << endl;
    }
   
    void visit(Rrx *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << "rrx\t" << register_name(s->rd->n) << ", " << register_name(s->rn->n) << endl;
    }

    void visit(Move *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             <<  "mov\t" << register_name(s->rd->n) << ", ";
    	if (s->constant == nullptr)
			cout << register_name(s->rs->n);
		else
			s->constant->accept(this);
		cout << endl;
    }

    void visit(Moves *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << "movs\t" << register_name(s->rd->n) << ", " << register_name(s->rn->n) << endl;
    }
    
    void visit(Msr *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << "msr\t" << register_special_name(s->rs->n) << ", " << register_name(s->rd->n) << endl;
    }

    void visit(Mrs *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << "mrs\t" << register_name(s->rd->n) << ", " << register_special_name(s->rs->n) << endl;
	}

    void visit(Push_pop *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << (s->push ? "push\t" : "pop\t") << register_name(s->r->n) << endl;
	}
    
    //------------------------------------------------------------------------------------------
    //	Directive
   
	void visit(DSection *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << ".section " << s->name << endl;
    }
	
    void visit(Byte *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << ((s->grain_size == 1) ? ".byte" : ".word") << '\t' << endl;
	}

    void visit(Ascii *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << ".ascii\t";
		for (auto p: *s->string_list)
			cout << p << endl;
	}
	
    void visit(Space *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << ".space\t";
		s->size->accept(this);
		cout << ", ";
		s->initial->accept(this);
		cout << endl;
    }

    void visit(Equ *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << ".equ\t" << s->name << ", ";
		s->value->accept(this);
		cout << endl;
    }

    void visit(Align *s) {
		cout << s->location.line << "\t" << s->Statement::to_string()
             << ".align\t";
		s->size->accept(this);
		cout << endl;
    }

	//------------------------------------------------------------------------------------------
	//	Expression
    
    void visit(Value *s) { cout << s->to_string(); }

    void visit(Identifier *s) { cout << s->to_string(); }

    void visit(Priority *s) { cout << s->to_string(); }

    void visit(Unary_expression *s) { cout << s->to_string(); }

    void visit(Binary_expression *s) { cout << s->to_string(); }

    void visit(Conditional_expression *s){ cout << s->to_string(); }
};
