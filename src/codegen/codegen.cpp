#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "./codegen.hpp"

void handle_instruction(const Token &token, std::vector<uint8_t> &section_data);
void handle_data(const Token &token, std::vector<uint8_t> &section_data);
void handle_label(const Token &token, std::vector<uint8_t> &section_data);
void handle_directive(const Token &token, std::vector<uint8_t> &section_data);
void handle_global(const Token &token, std::vector<uint8_t> &section_data);

// Helper function to write little-endian values
void write_le32(std::vector<uint8_t> &vec, uint32_t value)
{
    vec.push_back(static_cast<uint8_t>(value & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

void write_le64(std::vector<uint8_t> &vec, uint64_t value)
{
    vec.push_back(static_cast<uint8_t>(value & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 32) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 40) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 48) & 0xFF));
    vec.push_back(static_cast<uint8_t>((value >> 56) & 0xFF));
}

void handle_section(std::vector<Token> &tks, size_t &pos,
                    std::unordered_map<std::string, std::vector<uint8_t>> &sections)
{
    if (pos >= tks.size() || tks[pos].t_kind != TokenKind::SECTION)
    {
        return; // Invalid section token
    }

    pos++;

    if (pos >= tks.size())
    {
        return;
    }

    // Get section name (should be an ID or DIRECTIVE token like ".text")
    std::string section_name;
    if (tks[pos].t_kind == TokenKind::ID || tks[pos].t_kind == TokenKind::DIRECTIVE)
    {
        section_name = tks[pos].t_raw;
    }
    else
    {
        return; // Invalid section name
    }

    pos++; // Move past section name

    // Initialize section if it doesn't exist
    if (sections.find(section_name) == sections.end())
    {
        sections[section_name] = {};
    }

    // Process tokens until next section or end of file
    while (pos < tks.size() && tks[pos].t_kind != TokenKind::SECTION)
    {
        switch (tks[pos].t_kind)
        {
        case TokenKind::MOV:
        case TokenKind::XOR:
        case TokenKind::SYSCALL:
            handle_instruction(tks[pos], sections[section_name]);
            break;

        case TokenKind::LABEL:
            handle_label(tks[pos], sections[section_name]);
            break;

        case TokenKind::DIRECTIVE:
            handle_directive(tks[pos], sections[section_name]);
            break;

        case TokenKind::GLOBAL:
            handle_global(tks[pos], sections[section_name]);
            break;

        default:
            // Skip other tokens (REG, INT, COMMA, etc.)
            break;
        }
        pos++;
    }
    pos--; // Adjust position for outer loop increment
}

void handle_instruction(const Token &token, std::vector<uint8_t> &section_data)
{
    switch (token.t_kind)
    {
    case TokenKind::MOV:
        // MOV instruction - basic encoding for mov rax, immediate
        // This is a simplified encoding - real implementation needs operand parsing
        section_data.insert(section_data.end(), {0x48, 0xc7, 0xc0});       // mov rax, imm32
        section_data.insert(section_data.end(), {0x00, 0x00, 0x00, 0x00}); // 32-bit immediate placeholder
        break;

    case TokenKind::XOR:
        section_data.insert(section_data.end(), {0x48, 0x31, 0xc0}); // xor rax, rax
        break;

    case TokenKind::SYSCALL:
        section_data.insert(section_data.end(), {0x0f, 0x05}); // syscall
        break;

    default:
        // Unknown instruction - add NOP as placeholder
        section_data.push_back(0x90);
        break;
    }
}

void handle_data(const Token &token, std::vector<uint8_t> &section_data)
{
    // TODO
}

void handle_label(const Token &token, std::vector<uint8_t> &section_data)
{
    // Labels don't generate code, but we need to track their positions
    // The label name is in token.t_raw (includes the ':')
    // TODO: Implement symbol table management for labels

    // Labels are position markers, no actual bytes generated
    // The position should be tracked in a symbol table
    // For now, we'll just add a comment in the generated code
}

void handle_directive(const Token &token, std::vector<uint8_t> &section_data)
{
    std::string directive = token.t_raw;

    // Handle common assembler directives that start with '.'
    if (directive.find(".align") == 0)
    {
        // Alignment directive - pad with zeros to alignment boundary
        // Extract alignment value (simplified parsing)
        size_t space_pos = directive.find(' ');
        if (space_pos != std::string::npos)
        {
            std::string align_str = directive.substr(space_pos + 1);
            try
            {
                int alignment = std::stoi(align_str);
                size_t current_size = section_data.size();
                size_t padding = (alignment - (current_size % alignment)) % alignment;
                section_data.insert(section_data.end(), padding, 0x00);
            }
            catch (...)
            {
                // Invalid alignment, ignore
            }
        }
    }
    else if (directive.find(".byte") == 0 || directive.find(".db") == 0)
    {
        // Byte definition directive
        size_t space_pos = directive.find(' ');
        if (space_pos != std::string::npos)
        {
            std::string value_str = directive.substr(space_pos + 1);
            try
            {
                int value = std::stoi(value_str, nullptr, 0); // Support hex, dec, oct
                section_data.push_back(static_cast<uint8_t>(value & 0xFF));
            }
            catch (...)
            {
                section_data.push_back(0x00); // Default to 0 on parse error
            }
        }
    }
    else if (directive.find(".word") == 0 || directive.find(".dw") == 0)
    {
        // Word definition directive (16-bit)
        size_t space_pos = directive.find(' ');
        if (space_pos != std::string::npos)
        {
            std::string value_str = directive.substr(space_pos + 1);
            try
            {
                int value = std::stoi(value_str, nullptr, 0);
                // Little endian encoding
                section_data.push_back(static_cast<uint8_t>(value & 0xFF));
                section_data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
            }
            catch (...)
            {
                section_data.insert(section_data.end(), {0x00, 0x00});
            }
        }
    }
    else if (directive.find(".long") == 0 || directive.find(".dd") == 0)
    {
        // Long definition directive (32-bit)
        size_t space_pos = directive.find(' ');
        if (space_pos != std::string::npos)
        {
            std::string value_str = directive.substr(space_pos + 1);
            try
            {
                uint32_t value = static_cast<uint32_t>(std::stoul(value_str, nullptr, 0));
                // Little endian encoding
                section_data.push_back(static_cast<uint8_t>(value & 0xFF));
                section_data.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
                section_data.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
                section_data.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
            }
            catch (...)
            {
                section_data.insert(section_data.end(), {0x00, 0x00, 0x00, 0x00});
            }
        }
    }
}

void handle_global(const Token &token, std::vector<uint8_t> &section_data)
{
    // Global directives mark symbols as globally visible
    // They don't generate code, but should be recorded in symbol table
    // TODO: Implement symbol table management for global symbols
}

std::string GenCode(std::vector<Token> &tks)
{
    if (tks.empty() || tks[0].t_raw != "; x86_64-linux")
    {
        return "";
    }

    // ELF entry point address - standard location for executable
    const auto elf_entry = 0x400078;

    std::unordered_map<std::string, std::vector<uint8_t>> sections = {
        {"", std::vector<uint8_t>(0)}, // Null section (index 0)
        {".symtab", {}},               // Symbol table
        {".shstrtab", {}},             // Section header string table
        {".strtab", {}},               // String table
    };

    // Build string table for section names
    std::string shstrtab = "\0\0.symtab\0.shstrtab\0.strtab\0";
    std::vector<uint8_t> shstrtab_vec(shstrtab.begin(), shstrtab.end());
    sections[".shstrtab"] = shstrtab_vec;

    // ELF Header Construction (64 bytes total)
    std::vector<uint8_t> code_vec = {
        // ELF MAGIC NUMBER (4 bytes) - EI_MAG0 through EI_MAG3
        0x7f, 'E', 'L', 'F',

        // ELF IDENTIFICATION (12 bytes) - EI_CLASS through EI_PAD
        0x02, // EI_CLASS: ELFCLASS64 (64-bit objects)
        0x01, // EI_DATA: ELFDATA2LSB (little endian)
        0x01, // EI_VERSION: EV_CURRENT (current ELF version)
        0x00, // EI_OSABI: ELFOSABI_SYSV (System V ABI)
        0x00, // EI_ABIVERSION: ABI version (0 for System V)

        // EI_PAD: Padding bytes (7 bytes of zeros)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        // ELF HEADER FIELDS (48 bytes)

        // e_type: Object file type (2 bytes)
        0x02, 0x00, // ET_EXEC (executable file)

        // e_machine: Target architecture (2 bytes)
        0x3e, 0x00, // EM_X86_64 (AMD x86-64 architecture)

        // e_version: Object file version (4 bytes)
        0x01, 0x00, 0x00, 0x00, // EV_CURRENT

        // e_entry: Entry point virtual address (8 bytes, little endian)
        0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x400078

        // e_phoff: Program header table file offset (8 bytes)
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Offset 64 (after ELF header)

        // e_shoff: Section header table file offset (8 bytes) - will be updated
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        // e_flags: Processor-specific flags (4 bytes)
        0x00, 0x00, 0x00, 0x00, // No flags for x86_64

        // e_ehsize: ELF header size (2 bytes)
        0x40, 0x00, // 64 bytes

        // e_phentsize: Program header table entry size (2 bytes)
        0x38, 0x00, // 56 bytes (standard size for 64-bit)

        // e_phnum: Number of program header table entries (2 bytes)
        0x01, 0x00, // 1 program header

        // e_shentsize: Section header table entry size (2 bytes)
        0x40, 0x00, // 64 bytes (standard size for 64-bit)

        // e_shnum: Number of section header table entries (2 bytes) - will be updated
        0x00, 0x00,

        // e_shstrndx: Section header string table index (2 bytes)
        0x02, 0x00, // Index of .shstrtab section
    };

    // Process tokens to populate sections
    for (size_t i = 0; i < tks.size(); i++)
    {
        if (tks[i].t_kind == TokenKind::SECTION)
        {
            handle_section(tks, i, sections);
        }
    }

    // Add program header (56 bytes for 64-bit ELF)
    std::vector<uint8_t> program_header = {
        // p_type: Segment type (4 bytes)
        0x01, 0x00, 0x00, 0x00, // PT_LOAD (loadable segment)

        // p_flags: Segment flags (4 bytes)
        0x05, 0x00, 0x00, 0x00, // PF_R | PF_X (readable and executable)
    };

    // p_offset: Segment file offset (8 bytes)
    write_le64(program_header, 0);

    // p_vaddr: Segment virtual address (8 bytes)
    write_le64(program_header, 0x400000);

    // p_paddr: Segment physical address (8 bytes) - same as virtual for x86_64
    write_le64(program_header, 0x400000);

    // p_filesz: Segment size in file (8 bytes) - will be updated
    write_le64(program_header, 0);

    // p_memsz: Segment size in memory (8 bytes) - same as file size
    write_le64(program_header, 0);

    // p_align: Segment alignment (8 bytes)
    write_le64(program_header, 0x1000);

    // Add program header to code
    code_vec.insert(code_vec.end(), program_header.begin(), program_header.end());

    // Calculate section data offset (after ELF header + program header)
    uint64_t section_data_offset = code_vec.size();

    // Add section data
    std::vector<std::pair<std::string, uint64_t>> section_offsets;
    for (const auto &section : sections)
    {
        section_offsets.push_back({section.first, code_vec.size()});
        code_vec.insert(code_vec.end(), section.second.begin(), section.second.end());
    }

    // Calculate section header offset
    uint64_t section_header_offset = code_vec.size();

    // Update e_shoff in ELF header
    for (int i = 0; i < 8; i++)
    {
        code_vec[40 + i] = static_cast<uint8_t>((section_header_offset >> (i * 8)) & 0xFF);
    }

    // Update e_shnum in ELF header
    uint16_t num_sections = static_cast<uint16_t>(sections.size());
    code_vec[60] = static_cast<uint8_t>(num_sections & 0xFF);
    code_vec[61] = static_cast<uint8_t>((num_sections >> 8) & 0xFF);

    // Generate section headers (64 bytes each for 64-bit ELF)
    std::vector<std::string> section_order = {"", ".symtab", ".shstrtab", ".strtab"};

    // Add any user-defined sections
    for (const auto &section : sections)
    {
        if (std::find(section_order.begin(), section_order.end(), section.first) == section_order.end())
        {
            section_order.push_back(section.first);
        }
    }

    uint32_t string_table_offset = 1; // Start after null byte
    for (const std::string &section_name : section_order)
    {
        std::vector<uint8_t> section_header;

        // sh_name: Section name offset in string table (4 bytes)
        if (section_name.empty())
        {
            write_le32(section_header, 0); // Null section
        }
        else if (section_name == ".symtab")
        {
            write_le32(section_header, 1); // Offset in .shstrtab
        }
        else if (section_name == ".shstrtab")
        {
            write_le32(section_header, 9); // Offset in .shstrtab
        }
        else if (section_name == ".strtab")
        {
            write_le32(section_header, 19); // Offset in .shstrtab
        }
        else
        {
            write_le32(section_header, string_table_offset);
            string_table_offset += section_name.length() + 1;
        }

        // sh_type: Section type (4 bytes)
        if (section_name.empty())
        {
            write_le32(section_header, 0); // SHT_NULL
        }
        else if (section_name == ".symtab")
        {
            write_le32(section_header, 2); // SHT_SYMTAB
        }
        else if (section_name == ".shstrtab" || section_name == ".strtab")
        {
            write_le32(section_header, 3); // SHT_STRTAB
        }
        else if (section_name == ".text")
        {
            write_le32(section_header, 1); // SHT_PROGBITS
        }
        else
        {
            write_le32(section_header, 1); // SHT_PROGBITS (default)
        }

        // sh_flags: Section flags (8 bytes)
        if (section_name == ".text")
        {
            write_le64(section_header, 0x6); // SHF_ALLOC | SHF_EXECINSTR
        }
        else
        {
            write_le64(section_header, 0); // No flags
        }

        // sh_addr: Section virtual address (8 bytes)
        write_le64(section_header, 0);

        // sh_offset: Section file offset (8 bytes)
        auto it = std::find_if(section_offsets.begin(), section_offsets.end(),
                               [&section_name](const std::pair<std::string, uint64_t> &p)
                               {
                                   return p.first == section_name;
                               });
        if (it != section_offsets.end())
        {
            write_le64(section_header, it->second);
        }
        else
        {
            write_le64(section_header, 0);
        }

        // sh_size: Section size (8 bytes)
        auto section_it = sections.find(section_name);
        if (section_it != sections.end())
        {
            write_le64(section_header, section_it->second.size());
        }
        else
        {
            write_le64(section_header, 0);
        }

        // sh_link: Section header table index link (4 bytes)
        if (section_name == ".symtab")
        {
            write_le32(section_header, 3); // Link to .strtab (index 3)
        }
        else
        {
            write_le32(section_header, 0);
        }

        // sh_info: Section header extra information (4 bytes)
        if (section_name == ".symtab")
        {
            write_le32(section_header, 1); // Index of first non-local symbol
        }
        else
        {
            write_le32(section_header, 0);
        }

        // sh_addralign: Section alignment (8 bytes)
        if (section_name == ".symtab")
        {
            write_le64(section_header, 8); // 8-byte alignment for symbol table
        }
        else if (section_name == ".text")
        {
            write_le64(section_header, 16); // 16-byte alignment for code
        }
        else
        {
            write_le64(section_header, 1); // Default alignment
        }

        // sh_entsize: Section entry size (8 bytes)
        if (section_name == ".symtab")
        {
            write_le64(section_header, 24); // Size of ELF64 symbol table entry
        }
        else if (section_name == ".rela.text" || section_name.find(".rela") == 0)
        {
            write_le64(section_header, 24); // Size of ELF64 relocation entry with addend
        }
        else if (section_name == ".rel.text" || section_name.find(".rel") == 0)
        {
            write_le64(section_header, 16); // Size of ELF64 relocation entry without addend
        }
        else
        {
            write_le64(section_header, 0); // No fixed entry size
        }

        // Add section header to code
        code_vec.insert(code_vec.end(), section_header.begin(), section_header.end());
    }

    std::string code(reinterpret_cast<const char *>(code_vec.data()), code_vec.size());
    return code;
}
