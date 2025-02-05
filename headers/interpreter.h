#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

void intepret(Program * program, Program * parent);

void interpret_assignment(Assignment * assignment);
void interpret_if(ConditionalBranch * conditional_branch);
void interpret_while(WhileLoop * while_loop);
void interpret_for(ForLoop * for_loop);

Value interpret_expression(Expression * expression);

Value * cast(Value * value, Type type);

Value l_and(Value * left, Value * right);
Value l_or(Value * left, Value * right);
Value l_xor(Value * left, Value * right);

Value add(Value * left, Value * right);
Value substract(Value * left, Value * right);
Value multiply(Value * left, Value * right);
Value divide(Value * left, Value * right);
Value modulo(Value * left, Value * right);

Value greater_than(Value * left, Value * right);
Value greater_equal_than(Value * left, Value * right);
Value lower_than(Value * left, Value * right);
Value lower_equal_than(Value * left, Value * right);
Value equal_to(Value * left, Value * right);
Value not_equal_to(Value * left, Value * right);

#endif
