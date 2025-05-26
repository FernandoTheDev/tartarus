#ifndef TOKEN_HPP
#define TOKEN_HPP

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
    INT
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

#endif
