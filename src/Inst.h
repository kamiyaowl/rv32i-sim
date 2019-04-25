#pragma once

#include <string>
#include <stdint.h>
#include <functional>
#include <bitset>
#include "Reg.h"

using namespace std;
namespace sim {
    // 現在はrv32iのみ対応
    enum ImmType: char {
        None = '!', R='R', I='I', S='S', U='U',
    };
    struct Args {
        uint8_t opcode;
        uint8_t rs1;
        uint8_t rs2;
        uint8_t rd;
        uint8_t funct3;
        uint8_t funct7;
        uint32_t imm;
    };

    template<typename T, size_t XLEN>
    void parseArgs(T inst, ImmType immType, Args& args) {
        args.opcode = (inst >> 0) & 0x7f;
        switch(immType) {
            case ImmType::R:
                args.rd     = (inst >>  7) & 0x1f;
                args.funct3 = (inst >> 12) & 0x7;
                args.rs1    = (inst >> 15) & 0x1f;
                args.rs2    = (inst >> 20) & 0x1f;
                args.funct7 = (inst >> 25) & 0x7f;

                args.imm    = 0x0; 
                break;
            case ImmType::I:
                args.rd     = (inst >>  7) & 0x1f;
                args.funct3 = (inst >> 12) & 0x7;
                args.rs1    = (inst >> 15) & 0x1f;
                args.imm    = (inst >> 20) & 0x0fff;

                args.rs2    = 0x0;
                args.funct7 = 0x0;
                break;
            case ImmType::S:
                args.imm    = (((inst >> 25) & 0x7f) << 5) | ((inst >> 7) & 0x1f);
                args.funct3 = (inst >> 12) & 0x7;
                args.rs1    = (inst >> 15) & 0x1f;
                args.rs2    = (inst >> 20) & 0x1f;

                args.rd     = 0x0;
                args.funct7 = 0x0; 
                break;
            case ImmType::U:
                args.rd     = (inst >>  7) & 0x1f;
                args.imm    = (inst & 0xfffff000); 

                args.funct3 = 0x0;
                args.rs1    = 0x0;
                args.rs2    = 0x0;
                args.funct7 = 0x0;
                break;
            default:
                assert(false);
                break;
        }
    }

    template<typename T, size_t XLEN>
    using Process = void(Reg<T, XLEN>&, const Args&);

    template<typename T, size_t XLEN>
    class Inst {
        public:
            string name;
            uint8_t opcode;
            ImmType immType;
            function<Process<T, XLEN>> process;

            Inst() {}
            Inst(string name, uint8_t opcode, ImmType immType, function<Process<T, XLEN>> process):
            name(name), opcode(opcode), immType(immType), process(process) {
            }
            ~Inst() {}
            void run(Reg<T, XLEN>& reg, T inst) {
                Args args;
                parseArgs<T, XLEN>(inst, this->immType, args);
                assert(args.opcode == this->opcode);
                
                this->process(reg, args);
            }

    };
};