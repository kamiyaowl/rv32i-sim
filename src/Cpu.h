#pragma once
#include <stdio.h>
#include "Mem.h"
#include "Alu.h"
#include "ElfLoader.h"

namespace sim {
    namespace rv32i {
        using S    = int32_t;
        using U    = uint32_t;
        using ADDR = uint32_t;
          class Cpu {
            protected:
                Mem<S, ADDR> mem;
                Reg<S> reg;
                Alu<S, ADDR> alu;
            public:
                void reset() {
                    mem.reset();
                    reg.reset();
                    alu.reset();
                }
                void load_program(const std::string& elf_path){
                    uint32_t entryAddr = elf::load32(elf_path, [&](uint32_t addr, uint8_t data){
                        mem.write_byte(addr, data);
                    });
                    printf("[CPU] entryAddr:%08x\n", entryAddr);
                    reg.write_pc(entryAddr);
                }
                void step() {
                    auto pc = reg.read_pc();
                    auto inst = mem.read(pc);
                    printf("[CPU] pc:%d\tinst:%08x\n", pc, inst);
                    alu.run(reg, mem, inst);
                }
                void run(){
                    // TODO: 終了条件決める
                    while(1) {
                        step();
                    }
                }
        };
    };
};