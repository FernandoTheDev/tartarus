#pragma once

#include <string>
enum class TokenKind
{
    STRING,   // 0
    INT,      // 1
    COMMENT,  // 2
    COMMA,    // 3
    SECTION,  // 4
    XOR,      // 5
    MOV,      // 6
    GLOBAL,   // 7
    LABEL,    // 8
    SYSCALL,  // 9
    ID,       // 10
    REG,      // 11
    DIRECTIVE // 12
};

// Tokens: std::vector<Token>
struct Token
{
    TokenKind t_kind;
    std::string t_raw;
    u_int64_t Line;
    u_int64_t Start;
    u_int64_t End;
    Token(
        TokenKind t_kind,
        std::string t_raw,
        u_int64_t Line,
        u_int64_t Start,
        u_int64_t End);
};

void print_token(Token &tk);
