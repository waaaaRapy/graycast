#pragma once

#include <stdio.h>

#include "parser.h"

void generate_program(Program*);
void generate_function(Func*);
void generate_leftval(Node* node);
void generate(Node* node);
