#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

void interpret(Program * program, Program * parent);

void interpret_assignment(Assignment * assignment, Program * program);
void interpret_if(ConditionalBranch * conditional_branch, Program * program);
void interpret_while(WhileLoop * while_loop, Program * program);
void interpret_for(ForLoop * for_loop, Program * program);

void free_variable_dictionnary(VariableDictionnary * variable_dictionnary);
void add_to_variable_dictionnary(VariableDictionnary * dictionnary, Variable * variable);
void assign_variable(Variable * variable, Value * value);

Value * get_variable(Variable * variable, Program * program);

Value interpret_expression(Expression * expression, Program * program);
void interpret_statement(Statement * statement, Program * program);

Value * cast(Value * value, Type type);

Value u_minus(Value * val);
Value u_plus(Value * val);
Value u_not(Value * val);

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
