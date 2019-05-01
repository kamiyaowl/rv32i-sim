#pragma once

#include <stdint.h>
#include <string>
#include <functional>
#include <fstream>
#include <stdio.h>

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
        // https://codedocs.xyz/scarv/riscv-gnu-toolchain/linux_2elf_8h_source.html
        typedef struct elf32_hdr{
            unsigned char e_ident[EI_NIDENT];
            Elf32_Half    e_type;
            Elf32_Half    e_machine;
            Elf32_Word    e_version;
            Elf32_Addr    e_entry;  /* Entry point */
            Elf32_Off     e_phoff;
            Elf32_Off     e_shoff;
            Elf32_Word    e_flags;
            Elf32_Half    e_ehsize;
            Elf32_Half    e_phentsize;
            Elf32_Half    e_phnum;
            Elf32_Half    e_shentsize;
            Elf32_Half    e_shnum;
            Elf32_Half    e_shstrndx;
        } Elf32_Ehdr;
        typedef struct elf32_phdr{
            Elf32_Word    p_type;
            Elf32_Off     p_offset;
            Elf32_Addr    p_vaddr;
            Elf32_Addr    p_paddr;
            Elf32_Word    p_filesz;
            Elf32_Word    p_memsz;
            Elf32_Word    p_flags;
            Elf32_Word    p_align;
        } Elf32_Phdr;
        typedef struct elf32_shdr {
            Elf32_Word    sh_name;
            Elf32_Word    sh_type;
            Elf32_Word    sh_flags;
            Elf32_Addr    sh_addr;
            Elf32_Off     sh_offset;
            Elf32_Word    sh_size;
            Elf32_Word    sh_link;
            Elf32_Word    sh_info;
            Elf32_Word    sh_addralign;
            Elf32_Word    sh_entsize;
        } Elf32_Shdr;
            
        // Thanks: http://blue-9.hatenadiary.com/entry/2017/03/14/212929
        // Specs:  https://github.com/riscv/riscv-elf-psabi-doc/blob/master/riscv-elf.md
        uint32_t load32(const std::string& elf_path, std::function<void(uint32_t addr, uint32_t data)> write) {
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
            assert(hdr.e_phnum > 0); // Program Headerは存在するはず
            
            // Program Headerをみてメモリ上に展開
            for(int i = 0 ; i < hdr.e_phnum ; ++i) {
                Elf32_Phdr phdr = {};
                ifs.read((char*)(&phdr.p_type),   sizeof(phdr.p_type));
                ifs.read((char*)(&phdr.p_offset), sizeof(phdr.p_offset));
                ifs.read((char*)(&phdr.p_vaddr),  sizeof(phdr.p_vaddr));
                ifs.read((char*)(&phdr.p_paddr),  sizeof(phdr.p_paddr));
                ifs.read((char*)(&phdr.p_filesz), sizeof(phdr.p_filesz));
                ifs.read((char*)(&phdr.p_memsz),  sizeof(phdr.p_memsz));
                ifs.read((char*)(&phdr.p_flags),  sizeof(phdr.p_flags));
                ifs.read((char*)(&phdr.p_align),  sizeof(phdr.p_align));
                // とりあえず PT_LOAD(1)だけ考えとく
                if (phdr.p_type == 1) {
                    printf("[ElfLoader][LOAD] off:%08x vaddr:%08x paddr:%08x\n", phdr.p_offset, phdr.p_vaddr, phdr.p_paddr);
                    // p_offset : セグメント先頭へのファイル先頭からのオフセット
                    // p_vaddr  : メモリ上の仮想アドレス
                    // p_paddr  : 物理アドレスとして予約されている→使わない
                    // p_filesz : セグメントのファイルイメージのバイト数
                    // p_memsz  : 仮想メモリイメージでのバイト数→filesz使うので不問
                    // p_flags  : x/w/rフラグ→今回は無視
                    // p_align  : セグメントのアライン
                    assert(phdr.p_align > 0);
                    // ifstreamの現在位置を保存しておく
                    auto current = ifs.tellg();
                    ifs.seekg(phdr.p_offset, std::ifstream::beg);
                    // Callbackの実装がしょぼいのでがんばって1byteずつ読むよ...
                    for(int i = 0 ; i < phdr.p_filesz ; ++i) {
                        char buf;
                        ifs.read(&buf, 1);
                        write(phdr.p_vaddr + i, static_cast<uint8_t>(buf));
                    }
                    ifs.seekg(current, std::ifstream::beg);
                }
            }
            // おわり
            ifs.close();
            // EntryPointを返す
            return hdr.e_entry;
        }
    }
}