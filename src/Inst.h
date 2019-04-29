#pragma once

#include <string>
#include <stdint.h>
#include <functional>
#include <bitset>

#include "Reg.h"
#include "Mem.h"

using namespace std;
namespace sim {
    // 現在はrv32iのみ対応
    enum ImmType: char {
        None = '!', R='R', I='I', S='S', B='B', U='U', J='J',
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

    template<typename DATA, typename ADDR>
    using Process = void(Reg<DATA>&, Mem<DATA, ADDR>& mem, const Args&);

    template<typename DATA, typename ADDR>
    class Inst {
        public:
            string name;
            uint8_t opcode; // opcodeの一致を確認->parseArgsでfunct3,7(命令形式による)を取得→比較
            uint8_t funct3;
            uint8_t funct7;
            ImmType immType;
            function<Process<DATA, ADDR>> process;

            Inst() {}
            Inst(string name, uint8_t opcode, uint8_t funct3, uint8_t funct7, ImmType immType, function<Process<DATA, ADDR>> process):
                name(name), opcode(opcode), funct3(funct3), funct7(funct7), immType(immType), process(process) {
            }
            ~Inst() {}
            void run(Reg<DATA, ADDR>& reg, Mem<DATA, ADDR>& mem, DATA inst) {
                this->process(reg, mem, args);
            }

    };
};