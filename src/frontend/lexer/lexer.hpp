#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include "token.hpp"
#include <string>

std::vector<Token> lexer_tokenize(std::string &source);

Token lexer_mk_token(TokenKind kind, std::string &raw, u_int64_t line, u_int64_t s, u_int64_t e);

#endif
