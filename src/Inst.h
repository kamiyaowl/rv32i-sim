#pragma once

#include <string>
#include <stdint.h>
#include <functional>
#include <bitset>

#include "Reg.h"
#include "Mem.h"

#include "Logger.h"

using namespace std;
namespace sim {
    // 現在はrv32iのみ対応
    enum ImmType: char {
        None = '!', R='R', I='I', S='S', B='B', U='U', J='J',
    };
    struct Args {
        uint8_t  opcode;
        uint8_t  rs1;
        uint8_t  rs2;
        uint8_t  rd;
        uint8_t  funct3;
        uint8_t  funct7;
        uint32_t imm_raw; // 生の値
        int32_t  imm_signed; // 符号拡張済の値
    };
    // immを符号拡張する(bit_width: ビット幅12~32とか)
    int32_t convert_signed(uint32_t imm, size_t bit_width) {
        assert(bit_width > 0);
        assert(bit_width < 33);
        // 12bit -> 左
        size_t shift = (32 - bit_width);
        uint32_t shifted = imm << shift;
        int32_t  shifted_sign = static_cast<int32_t>(shifted);
        int32_t dst = shifted_sign >> shift;
        return dst;
    }
    template<typename T>
    void parse_args(T inst, ImmType immType, Args& args) {
        args.opcode = (inst >> 0) & 0x7f;
        switch(immType) {
            case ImmType::R:
                args.rd     = (inst >>  7) & 0x1f;
                args.funct3 = (inst >> 12) & 0x7;
                args.rs1    = (inst >> 15) & 0x1f;
                args.rs2    = (inst >> 20) & 0x1f;
                args.funct7 = (inst >> 25) & 0x7f;

                args.imm_raw    = 0x0; 
                args.imm_signed = 0;
                break;
            case ImmType::I:
                args.rd      = (inst >>  7) & 0x1f;
                args.funct3  = (inst >> 12) & 0x7;
                args.rs1     = (inst >> 15) & 0x1f;
                args.imm_raw = (inst >> 20) & 0x0fff;

                args.rs2     = 0x0;
                args.funct7  = 0x0;
                args.imm_signed = convert_signed(args.imm_raw, 12);
                break;
            case ImmType::S:
                args.imm_raw = 
                    (((inst >> 25) & 0x7f) << 5) | 
                    ((inst >> 7) & 0x1f);
                args.funct3  = (inst >> 12) & 0x7;
                args.rs1     = (inst >> 15) & 0x1f;
                args.rs2     = (inst >> 20) & 0x1f;

                args.rd      = 0x0;
                args.funct7  = 0x0; 
                args.imm_signed = convert_signed(args.imm_raw, 12);
                break;
            case ImmType::B:
                args.imm_raw = 
                    (((inst >> 31) & 0x01) << 12) |
                    (((inst >>  7) & 0x01) << 11) |
                    (((inst >> 25) & 0x3f) <<  5) |
                    (((inst >>  8) & 0x0f) <<  1);
                args.funct3  = (inst >> 12) & 0x7;
                args.rs1     = (inst >> 15) & 0x1f;
                args.rs2     = (inst >> 20) & 0x1f;

                args.rd      = 0x0;
                args.funct7  = 0x0; 
                args.imm_signed = convert_signed(args.imm_raw, 13);
                break;
            case ImmType::U:
                args.rd      = (inst >>  7) & 0x1f;
                args.imm_raw = (inst & 0xfffff000); 

                args.funct3  = 0x0;
                args.rs1     = 0x0;
                args.rs2     = 0x0;
                args.funct7  = 0x0;
                args.imm_signed = convert_signed(args.imm_raw, 32);
                break;
            case ImmType::J:
                args.imm_raw = 
                    (((inst >> 31) &  0x01) << 20) |
                    (((inst >> 12) &  0xff) << 12) |
                    (((inst >> 19) &  0x01) << 11) |
                    (((inst >> 21) & 0x3ff) << 1);
                args.rd      = (inst >>  7) & 0x1f;

                args.rs1     = 0x0;
                args.rs2     = 0x0;
                args.funct3  = 0x0;
                args.funct7  = 0x0; 
                args.imm_signed = convert_signed(args.imm_raw, 21);
                break;
            default:
                assert(false);
                break;
        }
    }
    template<typename DATA, typename ADDR>
    using Process = void(Reg<DATA>&, Mem<DATA, ADDR>& mem, const Args&);

    template<typename DATA, typename ADDR>
    class Inst {
        public:
            string name;
            uint8_t opcode; // opcodeの一致を確認->parse_argsでfunct3,7(命令形式による)を取得→比較
            uint8_t funct3;
            uint8_t funct7;
            ImmType immType;
            function<Process<DATA, ADDR>> process;

            Inst() {}
            Inst(string name, uint8_t opcode, uint8_t funct3, uint8_t funct7, ImmType immType, function<Process<DATA, ADDR>> process):
                name(name), opcode(opcode), funct3(funct3), funct7(funct7), immType(immType), process(process) {
            }
            ~Inst() {}
            void run(Reg<DATA>& reg, Mem<DATA, ADDR>& mem, DATA inst) {
                Args args;
                parse_args(inst, this->immType, args);
                sim::log::debug("\t[Inst][%s] rs1:%08x rs2:%08x rd:%08x imm:%08x(%d)\n", name.c_str(), args.rs1, args.rs2, args.rd, args.imm_raw, args.imm_signed);
                this->process(reg, mem, args);
            }

    };
};