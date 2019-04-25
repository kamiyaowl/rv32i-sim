#pragma once

#include <string>
#include <stdint.h>
#include <functional>
#include <bitset>
#include "Reg.h"

using namespace std;
namespace sim {
    enum ImmType: char {
        R='R', I='I', S='S', U='U',
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
    using Process = void(Reg<T, XLEN>&, const Args&);

    template<typename T, size_t XLEN>
    class Inst {
        protected:
            string name;
            uint8_t opcode;
            ImmType immType;
            function<Process<T, XLEN>> process;
        public:
            Inst() {}
            Inst(string name, uint8_t opcode, ImmType immType, function<Process<T, XLEN>> process):
            name(name), opcode(opcode), immType(immType), process(process) {

            }
            ~Inst() {}
    };
};