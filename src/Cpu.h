#pragma once
#include "Mem.h"
#include "Alu.h"
#include "ElfLoader.h"

#include "Logger.h"

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
                // TODO: 本当はCSR内に持つ
                bool halted;
            public:
                void reset() {
                    mem.reset();
                    reg.reset();
                    alu.reset();
                    halted = false;
                }
                void load_program(const std::string& elf_path){
                    uint32_t entryAddr = elf::load32(elf_path, [&](uint32_t addr, uint8_t data){
                        mem.write_byte(addr, data);
                    });
                    sim::log::system("[CPU] entryAddr:%08x\n", entryAddr);
                    reg.write_pc(entryAddr);
                    reg.debug_print();
                }
                void step() {
                    auto pc = reg.read_pc();
                    auto inst = mem.read(pc);
                    sim::log::debug("[CPU] pc:%08x inst:%08x\n", pc, inst);
                    if(!alu.run(reg, mem, inst)) {
                        sim::log::system("[CPU] Halted. pc:%08x inst:%08x\n", pc, inst);
                        halted = true;
                    }
                    reg.debug_print();
                }
                void run(){
                    while(!halted) {
                        step();
                    }
                }
        };
    };
};