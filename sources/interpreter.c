#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../headers/interpreter.h"

void interpret(Program * program, Program * parent){
  for (unsigned int current = 0; current < program->current_instruction; current++){
    if (program->instructions[current]->type == EXPRESSION){
      interpret_expression(program->instructions[current]->expression);
    } else {
      StatementType type = program->instructions[current]->statement->type;
      switch (type){
        case ASSIGNMENT:
          interpret_assignment(program->instructions[current]->statement->assignment, program);
          break;
        case CONDITIONAL_BRANCH:
          interpret_if(program->instructions[current]->statement->conditional_branch, program);
          break;
        case WHILE_LOOP:
          interpret_while(program->instructions[current]->statement->while_loop, program);
          break;
        case FOR_LOOP:
          interpret_for(program->instructions[current]->statement->for_loop, program);
          break;
        case PROGRAM:
          interpret(program->instructions[current]->statement->program, program);
      }
    }
  }
}

void interpret_assignment(Assignment * assignment, Program * parent);
void interpret_if(ConditionalBranch * conditional_branch, Program * parent);
void interpret_while(WhileLoop * while_loop, Program * parent);
void interpret_for(ForLoop * for_loop, Program * parent);

Value interpret_expression(Expression * expression);

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
        result.type = BOOL;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value * right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = left->float_value * right->float_value;
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
    result = multiply(left, casted_value);
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
        result.type = BOOL;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value / right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = left->float_value / right->float_value;
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
        result.type = BOOL;
      case INTEGER:
        result.type = INTEGER;
        result.integer_value = left->integer_value % right->integer_value;
        break;
      case FLOAT:
        result.type = FLOAT;
        result.float_value = fmodl(left->float_value, right->float_value);
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
