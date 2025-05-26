#include <iostream>
#include "src/frontend/lexer/lexer.hpp"
#include "src/common/Colours.hpp"
#include <fstream>
#include <filesystem>

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
    return 0;
} // I'm going to sleep soon (it's almost 10pm), I'll upload everything to github and you can do it locally
