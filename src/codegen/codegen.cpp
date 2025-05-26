#include "./codegen.hpp"
#include <cstddef>
#include <cstdint>
#include <string>


#include <unordered_map>
#include <vector>

//woops made this funcs istead of a clas ill fix the saturady or in the summer idk when ill be free
void handle_section(std::vector<Token>& tks, size_t& Pos){
    
}

std::string GenCode(std::vector<Token>& tks){
    //i skipped typecheck sorry not sorry 
    //we wont use elf.h cause we may be on windows 

    if(tks[0].t_raw == ";x86_64-linux"){
    std::unordered_map<std::string, std::vector<uint8_t>> Sections = {
        {"null" , {
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0,
            0x0, 0x0,0x0,0x0,0x0,0x0,0x0,0x0
        }},
        {".symtab" , {}},
        {".shstrtab", {}},
        {".strtab", {}},
    };

    std::vector<uint8_t> Code_Vec = {
        0x7f, //file type
        'E',
        'L',
        'F', 
        
        0x02, //class: 64 bit 
        0x01, //2s little edian 
        0x01, //elf version 1
        0x00, //linux abi - system v   
        0x00, //abi version 1
           
   
        //SOME RANDOM SHIT BRO:
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
       
        //Type: REL (2 bytes) <-- DEPENDS ON IF YOU USE DYNAMIC LINKING OR NOT
        0x01,
        0x00,

        //Machine: Advanced Micro Devices X86-64 (2 bytes)
        0x3E,
        0x00,

        //version: (4 bytes)
        0x01,
        0x00,
        0x00,
        0x00,
 
         //Entry Point: _start but we dont know so (8 bytes)
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
 
        //Start of program: 8 bytes  
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
 
         //Start of section headers: 8 bytes  
        0x40,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        
        //flags 4 bytes 
        0x00,
        0x00,
        0x00,
        0x00,
 
 
        //header size 2 bytes 
        0x40,
        0x00,
 
        //progarm header size 2 
        0x00,
        0x00,
        
        //number of program headers:
        0x00,
        0x00,
 
        //size of section haeders 2 bytes 
        0x40,
        0x00,
 
        //number of section header 2 bytes 
        0x00,
        0x00,
 
        //section header index 
        0x02,
        0x00,
                
    };

    for(size_t i = 0; i < tks.size(); i++){
        if(tks[i].t_kind == TokenKind::SECTION){
            handle_section(tks, i);
        }
    }
    //37 36 
    Code_Vec[37] = static_cast<uint8_t>(Sections.size());
    for(auto &i : Sections){
        Code_Vec.push_back(12);
        for(auto& i : i.second){
            Code_Vec.push_back(i);
        }
    }

    std::string Code(reinterpret_cast<const char*>(Code_Vec.data()), Code_Vec.size());

    return Code;
    };
    return "";
}
