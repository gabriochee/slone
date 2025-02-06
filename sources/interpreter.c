#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../headers/interpreter.h"

void interpret(Program * program, Program * parent){
  program->parent = parent;
  for (unsigned int current = 0; current < program->current_instruction; current++){
    if (program->instructions[current]->type == EXPRESSION){
      interpret_expression(program->instructions[current]->expression, program);
    } else {
      interpret_statement(program->instructions[current]->statement, program);
    }
  }
}

void interpret_statement(Statement * statement, Program * program){
      StatementType type = statement->type;
      switch (type){
        case ASSIGNMENT:
          interpret_assignment(statement->assignment, program);
          break;
        case CONDITIONAL_BRANCH:
          interpret_if(statement->conditional_branch, program);
          break;
        case WHILE_LOOP:
          interpret_while(statement->while_loop, program);
          break;
        case FOR_LOOP:
          interpret_for(statement->for_loop, program);
          break;
        case PROGRAM:
          interpret(statement->program, program);
        default:
          break;
      }
}

void interpret_assignment(Assignment * assignment, Program * program) {

  if (get_variable(assignment->variable, program) == NULL) {

    add_to_variable_dictionnary(program->variable_dictionary, assignment->variable);

    Value *val = malloc(sizeof(Value));

    *val = interpret_expression(assignment->value, program);

    assignment->variable->type = val->type;
    program->variable_dictionary->values[program->variable_dictionary->current - 1] = val;

  } else {
    for (unsigned int i = 0; i < program->variable_dictionary->current; i++) {
      if (strcmp(program->variable_dictionary->variables[i]->name, assignment->variable->name) == 0) {
        Value *val = malloc(sizeof(Value));
        *val = interpret_expression(assignment->value, program);

        assignment->variable->type = val->type;
        program->variable_dictionary->values[i] = val;
        return;
      }
    }
    interpret_assignment(assignment, program->parent);
  }
}

void interpret_if(ConditionalBranch * conditional_branch, Program * program){
  Value condition = interpret_expression(conditional_branch->expression, program);
  if (condition.type == BOOL){
    if (condition.bool_value){
      interpret(conditional_branch->true_branch, program);
    } else {
      interpret(conditional_branch->false_branch, program);
    }
  } else {
    fprintf(stderr, "ERREUR IF: Une erreur est survenue dans la condition.\n");
  }
}

void interpret_while(WhileLoop * while_loop, Program * program){
  Value condition = interpret_expression(while_loop->condition, program);

  if (condition.type == BOOL){
    while (condition.bool_value && condition.type == BOOL){
      interpret(while_loop->program, program);
      condition = interpret_expression(while_loop->condition, program);
    }
  } else {
    fprintf(stderr, "ERREUR WHILE : Une erreur est survenue dans la condition.\n");
  }
}

void interpret_for(ForLoop * for_loop, Program * program){
  Value condition;

  if (for_loop->initial_statement->type != EMPTY_STATEMENT){
    interpret_statement(for_loop->initial_statement, for_loop->program);
    condition = interpret_expression(for_loop->condition, for_loop->program);
  } else {
    condition = interpret_expression(for_loop->condition, program);
  }

  if (condition.type == BOOL){

    while (condition.bool_value && condition.type == BOOL){
      interpret_statement(for_loop->modifier, for_loop->program);
      interpret(for_loop->program, program);
      condition = interpret_expression(for_loop->condition, for_loop->program);
    }
  } else {
    fprintf(stderr, "ERREUR FOR : Une erreur est survenue dans la condition.\n");
  }

}

void add_to_variable_dictionnary(VariableDictionnary * dictionnary, Variable * variable) {
  if (dictionnary->current >= dictionnary->capacity) {
    dictionnary->variables = realloc(dictionnary->variables, (dictionnary->capacity *= 2) * sizeof(Variable *));
    dictionnary->values = realloc(dictionnary->values, (dictionnary->capacity) * sizeof(Value *));
  }

  if (dictionnary->variables != NULL && dictionnary->values != NULL) {
    dictionnary->variables[dictionnary->current++] = variable;
  }
}

Value * get_variable(Variable * variable, Program * program) {
  VariableDictionnary * dictionnary = program->variable_dictionary;

  for (unsigned int i = 0; i < dictionnary->current; i++) {
    if (strcmp(variable->name, dictionnary->variables[i]->name) == 0) {
      return dictionnary->values[i];
    }
  }
  
  if (program->parent != NULL){
    return get_variable(variable, program->parent);
  }

  return NULL;
}

Value interpret_expression(Expression * expression, Program * program){
  Value result;
  result.type = EMPTY;

  if (expression->type == VALUE){
    return *expression->value;
  }

  if (expression->type == VARIABLE){
    if (get_variable(expression->variable, program) == NULL){
      fprintf(stderr, "Variable '%s' introuvable.\n", expression->variable->name);
      return result;
    }

    return *get_variable(expression->variable, program);
  }

  if (expression->type == UNARY_OPERATION){
  	Value temp;
    Value (* unary_operator)(Value *);

    switch (expression->unary_operator->type){
      case UNARY_PLUS:
        unary_operator = &u_plus;
        break;
      case UNARY_MINUS:
        unary_operator = &u_minus;
        break;
      case LOGIC_NOT:
        unary_operator = &u_not;
        break;
    }

    if (expression->unary_operator->expression->type != VALUE){
      temp = interpret_expression(expression->unary_operator->expression, program);
    } else {
      temp = *expression->unary_operator->expression->value;
    }

    return unary_operator(&temp);
  }

  if (expression->type == BINARY_OPERATION){
  	Value temp_left, temp_right;
    Value (* binary_operator)(Value *, Value *);

    switch (expression->binary_operator->type){
	  case LOGIC_AND:
        binary_operator = &l_and;
        break;
      case LOGIC_OR:
        binary_operator = &l_or;
        break;
	  case LOGIC_XOR:
        binary_operator = &l_xor;
        break;
	  case ADD:
        binary_operator = &add;
        break;
	  case SUB:
        binary_operator = &substract;
        break;
	  case MULT:
        binary_operator = &multiply;
        break;
	  case DIV:
        binary_operator = &divide;
        break;
	  case MOD:
        binary_operator = &modulo;
        break;
	  case GREATER_THAN:
        binary_operator = &greater_than;
        break;
	  case GREATER_EQUAL_THAN:
        binary_operator = &greater_equal_than;
        break;
	  case LOWER_THAN:
        binary_operator = &lower_than;
        break;
	  case LOWER_EQUAL_THAN:
        binary_operator = &lower_equal_than;
        break;
	  case EQUAL_TO:
        binary_operator = &equal_to;
        break;
	  case NOT_EQUAL_TO:
        binary_operator = &not_equal_to;
        break;
    }

    if (expression->binary_operator->left_expression != VALUE){
      temp_left = interpret_expression(expression->binary_operator->left_expression, program);
    } else {
      temp_left = *expression->binary_operator->left_expression->value;
    }

    if (expression->binary_operator->right_expression != VALUE){
      temp_right = interpret_expression(expression->binary_operator->right_expression, program);
    } else {
      temp_right = *expression->binary_operator->right_expression->value;
    }

    return binary_operator(&temp_left, &temp_right);
  }

  return result;
}

Value * cast(Value * value, Type type){
  Value * casted_value = malloc(sizeof(Value));

  if (casted_value == NULL){
    return NULL;
  }

  casted_value->type = type;

  Type val_type = value->type;

  if (val_type == type){
    free(casted_value);
    return value;
  }

  if (val_type == UNSIGNED_INTEGER){
    switch (type){
      case INTEGER:
        casted_value->integer_value = (long long)value->unsigned_integer_value;
        return casted_value;
      case FLOAT:
        casted_value->float_value = (long double)value->unsigned_integer_value;
        return casted_value;
      default:
        break;
    }
  }

  if (val_type == INTEGER){
    switch (type){
      case UNSIGNED_INTEGER:
        casted_value->unsigned_integer_value = (unsigned long long)value->integer_value;
        return casted_value;
      case FLOAT:
        casted_value->float_value = (long double)value->integer_value;
        return casted_value;
      default:
      break;
    }
  }

  if (val_type == FLOAT){
    switch (type){
      case UNSIGNED_INTEGER:
        casted_value->unsigned_integer_value = (unsigned long long)value->float_value;
        return casted_value;
      case INTEGER:
        casted_value->integer_value = (long long)value->float_value;
        return casted_value;
      default:
        break;
    }
  }

  if (val_type == CHAR && type == STRING){
    casted_value->string_value = malloc(sizeof(char) * 2);

    if (casted_value->string_value == NULL){
      free(casted_value);
      return NULL;
    }

    casted_value->string_value[0] = value->char_value;
    casted_value->string_value[1] = '\0';

    return casted_value;
  }

  free(casted_value);
  return NULL;

}

Value u_minus(Value * val){
  Value result;
  result.type = EMPTY;

  if (val->type == INTEGER || val->type == FLOAT){
    result.type = val->type;
    if (val->type == INTEGER){
      result.integer_value = - val->integer_value;
    } else {
      result.float_value = - val->float_value;
    }
  }

  return result;
}

Value u_plus(Value * val){
  if (val->type == INTEGER || val->type == FLOAT){
    return *val;
  }

  Value empty;
  empty.type = EMPTY;

  return empty;
}

Value u_not(Value * val){
  Value result;
  result.type = EMPTY;

  if (val->type == BOOL){
     result.type = BOOL;
     result.bool_value = ! val->bool_value;
  }

  return result;
}

Value l_and(Value * left, Value * right){
  Value result;
  result.type = BOOL;
  result.bool_value = left->bool_value && right->bool_value;

  return result;
}

Value l_or(Value * left, Value * right){
  Value result;
  result.type = BOOL;
  result.bool_value = left->bool_value || right->bool_value;

  return result;
}

Value l_xor(Value * left, Value * right){
  Value result;
  result.type = BOOL;
  result.bool_value = (left->bool_value && !right->bool_value) || (!left->bool_value && right->bool_value);

  return result;

}

Value add(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = UNSIGNED_INTEGER;
        result.unsigned_integer_value = left->unsigned_integer_value + right->unsigned_integer_value;
        break;
        result.type = BOOL;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value + right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = left->float_value + right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    Value * casted_value = cast(left, FLOAT);
    result = add(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    Value * casted_value = cast(right, FLOAT);
    result = add(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = add(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = add(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value substract(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = UNSIGNED_INTEGER;
        result.unsigned_integer_value = left->unsigned_integer_value - right->unsigned_integer_value;
        break;
        result.type = BOOL;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value - right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = left->float_value - right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    Value * casted_value = cast(left, FLOAT);
    result = substract(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    Value * casted_value = cast(right, FLOAT);
    result = substract(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = substract(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = substract(left, casted_value);
    free(casted_value);
  }

  return result;
}
Value multiply(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = UNSIGNED_INTEGER;
        result.unsigned_integer_value = left->unsigned_integer_value * right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value * right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = left->float_value * right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    Value * casted_value = cast(left, FLOAT);
    result = multiply(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    Value * casted_value = cast(right, FLOAT);
    result = multiply(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = multiply(casted_value, right);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = multiply(left, casted_value);
    free(casted_value);
  }

  return result;
}
Value divide(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = UNSIGNED_INTEGER;
        result.unsigned_integer_value = left->unsigned_integer_value / right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value / right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = left->float_value / right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    Value * casted_value = cast(left, FLOAT);
    result = divide(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    Value * casted_value = cast(right, FLOAT);
    result = divide(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = divide(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = divide(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value modulo(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = UNSIGNED_INTEGER;
        result.unsigned_integer_value = left->unsigned_integer_value % right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value % right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = fmodl(left->float_value, right->float_value);
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    Value * casted_value = cast(left, FLOAT);
    result = modulo(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    Value * casted_value = cast(right, FLOAT);
    result = modulo(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = modulo(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = modulo(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value greater_than(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = BOOL;
        result.bool_value = left->unsigned_integer_value > right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = BOOL;
        result.bool_value = left->integer_value > right->integer_value;
        break;
      case FLOAT:
        result.type = BOOL;
        result.bool_value = left->float_value > right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(left, FLOAT);
    result = greater_than(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(right, FLOAT);
    result = greater_than(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = greater_than(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = greater_than(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value greater_equal_than(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = BOOL;
        result.bool_value = left->unsigned_integer_value >= right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = BOOL;
        result.bool_value = left->integer_value >= right->integer_value;
        break;
      case FLOAT:
        result.type = BOOL;
        result.bool_value = left->float_value >= right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(left, FLOAT);
    result = greater_equal_than(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(right, FLOAT);
    result = greater_equal_than(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = greater_equal_than(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = greater_equal_than(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value lower_than(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = BOOL;
        result.bool_value = left->unsigned_integer_value < right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = BOOL;
        result.bool_value = left->integer_value < right->integer_value;
        break;
      case FLOAT:
        result.type = BOOL;
        result.bool_value = left->float_value < right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(left, FLOAT);
    result = lower_than(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(right, FLOAT);
    result = lower_than(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = lower_than(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = lower_than(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value lower_equal_than(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.type = BOOL;
        result.bool_value = left->unsigned_integer_value <= right->unsigned_integer_value;
        break;
      case INTEGER:
        result.type = BOOL;
        result.bool_value = left->integer_value <= right->integer_value;
        break;
      case FLOAT:
        result.type = BOOL;
        result.bool_value = left->float_value <= right->float_value;
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(left, FLOAT);
    result = lower_equal_than(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(right, FLOAT);
    result = lower_equal_than(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = lower_equal_than(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = lower_equal_than(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value equal_to(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (left->type == right->type){
    result.type = BOOL;
    switch(left->type){
      case UNSIGNED_INTEGER:
        result.bool_value = left->unsigned_integer_value == right->unsigned_integer_value;
        break;
      case INTEGER:
        result.bool_value = left->integer_value == right->integer_value;
        break;
      case FLOAT:
        result.bool_value = left->float_value == right->float_value;
        break;
      case BOOL:
        result.bool_value = left->bool_value == right->bool_value;
        break;
      case CHAR:
        result.bool_value = left->char_value == right->char_value;
        break;
      case STRING:
        result.bool_value = !strcmp(left->string_value, right->string_value);
        break;
      default:
        break;
    }
  } else if ((left->type == UNSIGNED_INTEGER || left->type == INTEGER) && right->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(left, FLOAT);
    result = equal_to(casted_value, right);
    free(casted_value);
  } else if ((right->type == UNSIGNED_INTEGER || right->type == INTEGER) && left->type == FLOAT) {
    result.type = BOOL;
    Value * casted_value = cast(right, FLOAT);
    result = equal_to(left, casted_value);
    free(casted_value);
  } else if (right->type == UNSIGNED_INTEGER && left->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(left, UNSIGNED_INTEGER);
    result = equal_to(left, casted_value);
    free(casted_value);
  } else if (left->type == UNSIGNED_INTEGER && right->type == INTEGER) {
    result.type = BOOL;
    Value * casted_value = cast(right, UNSIGNED_INTEGER);
    result = equal_to(left, casted_value);
    free(casted_value);
  }

  return result;
}

Value not_equal_to(Value * left, Value * right){
  Value result;
  result.type = EMPTY;

  if (equal_to(left, right).type != EMPTY){
    result.type = BOOL;
    result.bool_value = !equal_to(left, right).bool_value;
  }

  return result;
}
