#include "lexer.hpp"
#include "token.hpp"
#include <cstdint>
#include <ctype.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

std::vector<Token> lexer_tokenize(std::string &source)
{
    std::vector<Token> tokens;
    u_int64_t offset = 0;
    u_int64_t line = 1;
    u_int64_t col = 1;
    bool err = false;
    while (offset < source.size())
    {
        switch (source[offset])
        {
        case '\n':
            line++;
            offset++;
            col = 1;
            break;
        case ' ':
        case '\t':
            col++;
            offset++;
            break;
        case ',':
            tokens.push_back(Token(TokenKind::COMMA, ",", line, col, col));
            col++;
            offset++;
            break;
        case ';':
        {
            uint64_t start = col;
            std::string buffer;
            while (source[offset] != '\n' && source.length() > offset)
            {
                buffer += source[offset];
                col++;
                offset++;
            }
            tokens.push_back(Token(TokenKind::COMMENT, buffer, line, start, col));
            break;
        } // note when using cases if a varible is devlared inside of them add {}
        default:
            if (isalpha(source[offset]) || source[offset] == '_' || source[offset] == '.')
            {
                std::string buffer;
                u_int64_t start = col;
                while (isalnum(source[offset]) || source[offset] == '_' || source[offset] == '.' && offset < source.size())
                {
                    buffer += source[offset];
                    offset++;
                    col++;
                }

                // std::cout << "Logs. Buffer: " << buffer << "\n";

                std::unordered_map<std::string, TokenKind> Als = {
                    // 64 bit
                    {"rdi", TokenKind::REG},
                    {"rax", TokenKind::REG},

                    // 32 bit
                    {"edi", TokenKind::REG},
                    {"eax", TokenKind::REG},
                    // 16 bit
                    {"di", TokenKind::REG},
                    {"ax", TokenKind::REG},

                    // 8 bit
                    {"dil", TokenKind::REG},
                    {"al", TokenKind::REG},

                    {"section", TokenKind::SECTION},
                    {"xor", TokenKind::XOR},

                    {"syscall", TokenKind::SYSCALL},
                    {"mov", TokenKind::MOV},

                    {"global", TokenKind::GLOBAL}};
                if (Als.find(buffer) != Als.end())
                {
                    tokens.push_back(Token(Als[buffer], buffer, line, start, col));
                }
                else if (source[offset] == ':')
                {
                    col++;
                    offset++;
                    tokens.push_back(Token(TokenKind::LABEL, buffer + ":", line, start, col));
                }
                else if (buffer[0] == '.')
                {
                    tokens.push_back(Token(TokenKind::DIRECTIVE, buffer, line, start, col));
                }
                else
                {
                    tokens.push_back(Token(TokenKind::ID, buffer, line, start, col));
                }
            }
            else if (isdigit(source[offset]))
            {
                std::string buffer;
                u_int64_t start = col;
                while (isdigit(source[offset]) && offset < source.size())
                {
                    buffer += source[offset];
                    offset++;
                    col++;
                }
                tokens.push_back(Token(TokenKind::INT, buffer, line, start, col));
            }
            else
            {
                // TODO: Handle Error
                std::cerr << line << ": Unknow Token '" << source[offset] << "'\n";
                offset++;
                col++;
                err = true;
            }
            break;
        }
    }
    if (err == true)
    {
        exit(1);
    }
    return tokens;
}
