#include <cstddef>
#include <cstring>
#include <ios>
#include <iostream>
#include "src/codegen/codegen.hpp"
#include "src/frontend/lexer/lexer.hpp"
#include "src/common/Colours.hpp"
#include "src/frontend/lexer/token.hpp"
#include <fstream>
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>
// TODO: add brazllian lang or potugessue if a flag if passes eg | Ok
// #define Lang 1 <- Eng
// #define Lang 2 <- portugese

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cerr << BrightWhite << "tartarus:" << BrightRed << " fatal error: " << Reset << "no input files\n";
        exit(1);
    }

    std::ifstream File(argv[1]);
    if (!File.is_open())
    {
        std::cerr << BrightWhite << "tartarus:" << BrightRed << " fatal error: " << Reset << "'" << argv[1] << "' no such file or directory\n";
        exit(1);
    }
    if (std::filesystem::is_directory(argv[1]))
    {
        std::cerr << BrightWhite << "tartarus:" << BrightRed << " fatal error: " << Reset << "'" << argv[1] << "' is a directory\n";
        exit(1);
    }

    std::string Code;
    {
        std::string Line; 
        while(std::getline(File, Line)){
            Code+=Line+"\n";
        }
    }
    std::string Name = "out.o";

    for(size_t i = 2; i < argc; i++){
        if(!strcmp("-o", argv[i])){
            Name = argv[i + 1];
        }
    }


    File.close();
    std::vector<Token> Tokens = lexer_tokenize(Code);
    for(auto& i : Tokens){
        print_token(i); 
    }
       
    std::string Obj_Code = GenCode(Tokens);
    std::ofstream Obj(Name, std::ios::binary);
    if(Obj){
        Obj.write(Obj_Code.data(), Obj_Code.size());
    }
    return 0;
} // I'm going to sleep soon (it's almost 10pm), I'll upload everything to github and you can do it locally i actually have to go too  lol just doing this one thing
