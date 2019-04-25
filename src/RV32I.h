#pragma once
#include <map>
#include "Reg.h"
#include "Inst.h"

namespace sim {

    namespace rv32i {
        using T = uint32_t;
        const size_t XLEN = 32;

        const Inst<T, XLEN> add("add", 0b0110011, ImmType::R, [](Reg<T, XLEN>& reg, const Args args){
        });
        const std::map<uint8_t, Inst<T, XLEN>> instructions = {
            { add.opcode, add },
        };

    }
};