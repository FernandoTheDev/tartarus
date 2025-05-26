#pragma once
#include <vector>
#include <string>
#include "../frontend/lexer/token.hpp"

std::string GenCode(std::vector<Token>& tks);
