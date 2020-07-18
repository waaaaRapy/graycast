#pragma once

#include <stdio.h>

#include "parser.h"

void generate_main(Node* ast);
void generate_function(char* name, Node* body);
void generate_leftval(Node* node);
void generate(Node* node);
