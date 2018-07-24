#include "visitor.h"
#include "instruction.h"
#include "directive.h"

using namespace ast;

class Code_generator: public Visitor {

    //	Statement

public:
    void visit(Statement_empty *) {}

    //------------------------------------------------------------------------------------------
    //	Instruction

    void visit(Load_store_direct *s);
    void visit(Load_store_indirect *s);
    void visit(Branch *s);
    void visit(Shift *s);
    void visit(Rrx *s);
    void visit(Arith *s);
    void visit(Logic *s);
    void visit(Not *s);
    void visit(Move *s);
    void visit(Moves *s);
    void visit(Compare *s);
    void visit(Msr *s);
    void visit(Mrs *s);
    void visit(Push_pop *s);

    //------------------------------------------------------------------------------------------
    //	Directive

    void visit(DSection *) {}
    void visit(Ascii *) {}
    void visit(Equ *) {}
    void visit(Space *s);
    void visit(Align *s);
    void visit(Byte *s);

    //------------------------------------------------------------------------------------------
    //	Expression

    void visit(Value *) {}
    void visit(Identifier *) {}
    void visit(Priority *) {}
    void visit(Unary_expression *) {}
    void visit(Binary_expression *) {}
    void visit(Conditional_expression *) {}
};

