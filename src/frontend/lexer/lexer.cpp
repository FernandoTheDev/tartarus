#include "lexer.hpp"
#include <ctype.h>
#include <string>

std::vector<Token> lexer_tokenize(std::string &source)
{
    std::vector<Token> tokens;
    u_int64_t offset = 0;
    u_int64_t line = 1;
    u_int64_t col = 0;
    while (offset < source.size())
    {
        char c = source[offset];
        switch (c)
        {
        case '\n':
            line++;
            offset++;
            col = 0;
            break; //  doesn't break the while?
        default:
            if (isalnum(c))
            {
                std::string buffer;
                u_int64_t start = col;
                while (isalnum(c) && offset < source.size())
                {
                    buffer += c; // this is good?
                    offset++;
                    col++;
                    c = source[offset];
                }

                tokens.push_back(Token(TokenKind::ID, buffer, line, start, col)); // ok? is good?
            }
            break;
        }
    }

    return tokens;
}

// save file!
//  use Tokens.emplace()
//  for what? so we dont have to use a constructor or function we just pass in th args | Ok (never used)
//? save the file | done hmm look at the terminal though
//  how many mistakes
//  why?
