#include "./token.hpp"

Token::Token(
    TokenKind t_kind,
    std::string t_raw,
    u_int64_t Line,
    u_int64_t Start,
    u_int64_t End)
{
    this->t_kind = t_kind,
    this->t_raw = t_raw,
    this->Line = Line;
    this->Start = Start;
    this->End = End;
}
