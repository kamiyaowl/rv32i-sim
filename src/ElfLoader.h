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
        typedef struct elf32_hdr{
            unsigned char e_ident[EI_NIDENT];
            Elf32_Half    e_type;
            Elf32_Half    e_machine;
            Elf32_Word    e_version;
            Elf32_Addr    e_entry;  /* Entry point */
            Elf32_Off e_phoff;
            Elf32_Off e_shoff;
            Elf32_Word    e_flags;
            Elf32_Half    e_ehsize;
            Elf32_Half    e_phentsize;
            Elf32_Half    e_phnum;
            Elf32_Half    e_shentsize;
            Elf32_Half    e_shnum;
            Elf32_Half    e_shstrndx;
        } Elf32_Ehdr;
            
        // Thanks: http://blue-9.hatenadiary.com/entry/2017/03/14/212929
        // Specs:  https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md
        void load(const std::string& elf_path, std::function<void(uint32_t addr, uint32_t data)> write) {
            std::ifstream ifs(elf_path, ifstream::in | ifstream::binary);
            assert(ifs);

            // ELF Headerを読み出す
            Elf32_Ehdr hdr = {};
            ifs.read((char*)(&hdr.e_ident[0]), EI_NIDENT);
            ifs.read((char*)(&hdr.e_type),      sizeof(hdr.e_type));
            ifs.read((char*)(&hdr.e_machine),   sizeof(hdr.e_machine));
            ifs.read((char*)(&hdr.e_version),   sizeof(hdr.e_version));

            ifs.read((char*)(&hdr.e_entry),     sizeof(hdr.e_entry));
            ifs.read((char*)(&hdr.e_phoff),     sizeof(hdr.e_phoff));
            ifs.read((char*)(&hdr.e_shoff),     sizeof(hdr.e_shoff));
            ifs.read((char*)(&hdr.e_flags),     sizeof(hdr.e_flags));
            ifs.read((char*)(&hdr.e_ehsize),    sizeof(hdr.e_ehsize));
            ifs.read((char*)(&hdr.e_phentsize), sizeof(hdr.e_phentsize));
            ifs.read((char*)(&hdr.e_phnum),     sizeof(hdr.e_phnum));
            ifs.read((char*)(&hdr.e_shentsize), sizeof(hdr.e_shentsize));
            ifs.read((char*)(&hdr.e_shnum),     sizeof(hdr.e_shnum));
            ifs.read((char*)(&hdr.e_shstrndx),  sizeof(hdr.e_shstrndx));
            assert(hdr.e_ident[0] == 0x7f);
            assert(hdr.e_ident[1] == 'E');
            assert(hdr.e_ident[2] == 'L');
            assert(hdr.e_ident[3] == 'F');
            assert(hdr.e_type == 2); // ET_EXEC
            assert(hdr.e_machine == 243); // EM_RISCV (243) for RISC-V ELF files. We only support RISC-V v2 family ISAs, this support is implicit.
            assert(hdr.e_entry > 0); // 実行可能な場合、エントリポイントの仮想アドレス。今回は実行ファイルのみサポート
            
            // TODO:ph Program Headerのp_type=1(LOAD)をメモリ上に展開
            

            // おわり
            ifs.close();
        }
    }
}