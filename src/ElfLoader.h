#pragma once

#include <stdint.h>
#include <string>
#include <functional>
#include <fstream>

// elf.hから一部引用
typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t	    Elf32_Sword;
typedef uint32_t	Elf32_Word;

typedef uint64_t	Elf64_Addr;
typedef uint16_t	Elf64_Half;
typedef int16_t     Elf64_SHalf;
typedef uint64_t	Elf64_Off;
typedef int32_t	    Elf64_Sword;
typedef uint32_t	Elf64_Word;
typedef uint64_t	Elf64_Xword;
typedef int64_t	    Elf64_Sxword;

#define EI_NIDENT        16

using namespace std;
namespace sim {
    namespace elf {
        typedef struct elf64_hdr {
            unsigned char e_ident[EI_NIDENT];     /* ELF "magic number" */
            Elf64_Half e_type;
            Elf64_Half e_machine;
            Elf64_Word e_version;
            Elf64_Addr e_entry;           /* Entry point virtual address */
            Elf64_Off e_phoff;            /* Program header table file offset */
            Elf64_Off e_shoff;            /* Section header table file offset */
            Elf64_Word e_flags;
            Elf64_Half e_ehsize;
            Elf64_Half e_phentsize;
            Elf64_Half e_phnum;
            Elf64_Half e_shentsize;
            Elf64_Half e_shnum;
            Elf64_Half e_shstrndx;
        } Elf64_Ehdr;

        // Thanks: http://blue-9.hatenadiary.com/entry/2017/03/14/212929
        void load(const std::string& elf_path, std::function<void(uint32_t addr, uint32_t data)> write) {
            std::ifstream ifs(elf_path, ifstream::in | ifstream::binary);
            assert(ifs);

            // ELF Headerを読み出す
            Elf64_Ehdr hdr;
            ifs.read((char*)(&hdr.e_ident[0]), EI_NIDENT);
            assert(hdr.e_ident[0] == 0x7f);
            assert(hdr.e_ident[1] == 'E');
            assert(hdr.e_ident[2] == 'L');
            assert(hdr.e_ident[3] == 'F');


            // おわり
            ifs.close();
        }
    }
}