#pragma once
#include <map>
#include <functional>
#include "Reg.h"
#include "Inst.h"

namespace sim {
    template<typename T, size_t XLEN>
    inline Inst<T, XLEN> gen_r_inst(string name, uint8_t funct3, uint8_t funct7, function<T(T,T)> p) {
        return Inst<T, XLEN>(
            name,
            0b0110011,
            funct3,
            funct7,
            ImmType::R,
            [&](Reg<T, XLEN>& reg, const Args args){
                auto r1 = reg.read(args.rs1);
                auto r2 = reg.read(args.rs2);

                auto dst = p(r1, r2);
                
                reg.write(args.rd, dst);
            }
        );
    }
    namespace rv32i {
        // 普通は符号付き<S>で取り扱って、明示的にunsignedが必要なところだけstatic_cast<U>で実装する
        using S = int32_t;
        using U = uint32_t;
        const size_t XLEN = 32;
        // Integer Register-Register Operations
        const Inst<S, XLEN> inst_add  = gen_r_inst<S, XLEN>("add" , 0b000, 0b0000000, [](S a, S b) { return a + b; });
        const Inst<S, XLEN> inst_sub  = gen_r_inst<S, XLEN>("sub" , 0b000, 0b0100000, [](S a, S b) { return a - b; });
        const Inst<S, XLEN> inst_sll  = gen_r_inst<S, XLEN>("sll" , 0b001, 0b0000000, [](S a, S b) { assert(b > -1); return static_cast<U>(a) << b; });
        const Inst<S, XLEN> inst_slt  = gen_r_inst<S, XLEN>("slt" , 0b010, 0b0000000, [](S a, S b) { return a > b ? 0x1 : 0x0; });
        const Inst<S, XLEN> inst_sltu = gen_r_inst<S, XLEN>("sltu", 0b011, 0b0000000, [](S a, S b) { return static_cast<U>(a) > static_cast<U>(b) ? 0x1 : 0x0; });
        const Inst<S, XLEN> inst_xor  = gen_r_inst<S, XLEN>("xor" , 0b100, 0b0000000, [](S a, S b) { return static_cast<U>(a) ^ static_cast<U>(b); });
        const Inst<S, XLEN> inst_srl  = gen_r_inst<S, XLEN>("srl" , 0b101, 0b0000000, [](S a, S b) { assert(b > -1); return static_cast<U>(a) >> b; });
        const Inst<S, XLEN> inst_sra  = gen_r_inst<S, XLEN>("sra" , 0b101, 0b0100000, [](S a, S b) { assert(b > -1); return a >> b; });
        const Inst<S, XLEN> inst_or   = gen_r_inst<S, XLEN>("or"  , 0b110, 0b0000000, [](S a, S b) { return static_cast<U>(a) | static_cast<U>(b); });
        const Inst<S, XLEN> inst_and  = gen_r_inst<S, XLEN>("and" , 0b111, 0b0000000, [](S a, S b) { return static_cast<U>(a) & static_cast<U>(b); });
    }
};