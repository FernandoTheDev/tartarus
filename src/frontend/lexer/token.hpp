#pragma once 

#include <string>
enum class TokenKind
{
    // _section, ...
    ID,

    // mov, ...
    OP,

    // rdi, rxi, ...
    REG,

    // "Hi"
    STRING,

    // 0..9
    INT,
    
    COMMENT,
    COMMA, 
    SECTION,
    XOR, 
    MOV, 
    GLOBAL, 
    LABLE,
    SYSCALL,
};

// Tokens
// std::vector<Token>
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

void print_token(Token& tk);
