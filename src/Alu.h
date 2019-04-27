#pragma once
#include <map>
#include <functional>
#include "Reg.h"
#include "Inst.h"

namespace sim {
    template<typename T, size_t XLEN>
    inline Inst<T, XLEN> alu_32i_r_inst(string name, uint8_t funct3, uint8_t funct7, function<T(T,T)> p) {
        return Inst<T, XLEN>(
            name,
            0b0110011,
            funct3,
            funct7,
            ImmType::R,
            [&](Reg<T, XLEN>& reg, const Args args) {
                auto r1 = reg.read(args.rs1);
                auto r2 = reg.read(args.rs2);
                auto dst = p(r1, r2);
                reg.write(args.rd, dst);
                reg.incr_pc();
            }
        );
    }
    template<typename T, size_t XLEN>
    inline Inst<T, XLEN> alu_32i_i_inst(string name, uint8_t funct3, uint8_t funct7, function<T(T,T)> p) {
        return Inst<T, XLEN>(
            name,
            0b0010011,
            funct3,
            funct7, // invalid(srai, srliの見分けるためだけに必要、通常は0x0)
            ImmType::I,
            [&](Reg<T, XLEN>& reg, const Args args) {
                auto r1 = reg.read(args.rs1);
                auto imm = args.imm;
                auto dst = p(r1, imm);
                reg.write(args.rd, dst);
                reg.incr_pc();
            }
        );
    }
    template<typename T, size_t XLEN>
    inline Inst<T, XLEN> alu_32i_b_inst(string name, uint8_t funct3, function<bool(T,T)> cond) {
        return Inst<T, XLEN>(
            name,
            0b1100011,
            funct3,
            0x0, // invalid
            ImmType::B,
            [&](Reg<T, XLEN>& reg, const Args args) {
                auto r1 = reg.read(args.rs1);
                auto r2 = reg.read(args.rs2);
                if (cond(r1, r2)) {
                    auto dst = reg.read_pc() + args.imm;
                    reg.write_pc(dst);
                } else {
                    reg.incr_pc();
                }
            }
        );
    }
    namespace rv32i {
        // 普通は符号付き<S>で取り扱って、明示的にunsignedが必要なところだけstatic_cast<U>で実装する
        using S = int32_t;
        using U = uint32_t;
        const size_t XLEN = 32;
        const vector<Inst<S, XLEN>> instructions = {
            // OP-Reg
            alu_32i_r_inst<S, XLEN>("add"   , 0b000, 0b0000000, [](S a, S b) { return a + b; }),
            alu_32i_r_inst<S, XLEN>("sub"   , 0b000, 0b0100000, [](S a, S b) { return a - b; }),
            alu_32i_r_inst<S, XLEN>("sll"   , 0b001, 0b0000000, [](S a, S b) { assert(b > -1); return static_cast<U>(a) << b; }),
            alu_32i_r_inst<S, XLEN>("slt"   , 0b010, 0b0000000, [](S a, S b) { return a < b ? 0x1 : 0x0; }),
            alu_32i_r_inst<S, XLEN>("sltu"  , 0b011, 0b0000000, [](S a, S b) { return static_cast<U>(a) < static_cast<U>(b) ? 0x1 : 0x0; }),
            alu_32i_r_inst<S, XLEN>("xor"   , 0b100, 0b0000000, [](S a, S b) { return static_cast<U>(a) ^ static_cast<U>(b); }),
            alu_32i_r_inst<S, XLEN>("srl"   , 0b101, 0b0000000, [](S a, S b) { assert(b > -1); return static_cast<U>(a) >> b; }),
            alu_32i_r_inst<S, XLEN>("sra"   , 0b101, 0b0100000, [](S a, S b) { assert(b > -1); return a >> b; }),
            alu_32i_r_inst<S, XLEN>("or"    , 0b110, 0b0000000, [](S a, S b) { return static_cast<U>(a) | static_cast<U>(b); }),
            alu_32i_r_inst<S, XLEN>("and"   , 0b111, 0b0000000, [](S a, S b) { return static_cast<U>(a) & static_cast<U>(b); }),
            // OP-IMM
            alu_32i_i_inst<S, XLEN>("slli"  , 0b001, 0b0000000, [](S a, S imm) { assert((imm & 0xfe0) == 0x000); assert(imm > -1); return static_cast<U>(a) << (imm & 0x1f); }), // shamt==imm[24:20]
            alu_32i_i_inst<S, XLEN>("srli"  , 0b101, 0b0000000, [](S a, S imm) { assert((imm & 0xfe0) == 0x000); assert(imm > -1); return static_cast<U>(a) >> (imm & 0x1f); }), // shamt==imm[24:20]
            alu_32i_i_inst<S, XLEN>("srai"  , 0b101, 0b0100000, [](S a, S imm) { assert((imm & 0xfe0) == 0x400); assert(imm > -1); return a >> (imm & 0x1f); }), // shamt==imm[24:20]
            alu_32i_i_inst<S, XLEN>("addi"  , 0b000, 0b0000000, [](S a, S imm) { return a + imm; }),
            alu_32i_i_inst<S, XLEN>("slti"  , 0b010, 0b0000000, [](S a, S imm) { return a < imm ? 0x1 : 0x0; }),
            alu_32i_i_inst<S, XLEN>("sltiu" , 0b011, 0b0000000, [](S a, S imm) { return static_cast<U>(a) < static_cast<U>(imm) ? 0x1 : 0x0; }),
            alu_32i_i_inst<S, XLEN>("xori"  , 0b100, 0b0000000, [](S a, S imm) { return static_cast<U>(a) ^ static_cast<U>(imm); }),
            alu_32i_i_inst<S, XLEN>("ori"   , 0b110, 0b0000000, [](S a, S imm) { return static_cast<U>(a) | static_cast<U>(imm); }),
            alu_32i_i_inst<S, XLEN>("andi"  , 0b111, 0b0000000, [](S a, S imm) { return static_cast<U>(a) & static_cast<U>(imm); }),
            // Store
            // TODO:

            // Load
            // TODO:

            // Branch
            alu_32i_b_inst<S, XLEN>("beq"   , 0b000, [](S a, S b) { return a == b; }),
            alu_32i_b_inst<S, XLEN>("bne"   , 0b001, [](S a, S b) { return a != b; }),
            alu_32i_b_inst<S, XLEN>("blt"   , 0b100, [](S a, S b) { return a <  b; }),
            alu_32i_b_inst<S, XLEN>("bge"   , 0b101, [](S a, S b) { return a >  b; }),
            alu_32i_b_inst<S, XLEN>("bltu"  , 0b110, [](S a, S b) { return static_cast<U>(a) <   static_cast<U>(b); }),
            alu_32i_b_inst<S, XLEN>("bgeu"  , 0b111, [](S a, S b) { return static_cast<U>(a) >=  static_cast<U>(b); }),
            // Jump
            Inst<S, XLEN>(
                "jalr",
                0b1100111,
                0x0, // invalid
                0x0, // invalid
                ImmType::I,
                [](Reg<S, XLEN>& reg, const Args args) {
                    reg.write(args.rd, reg.read_pc() + 1);

                    auto rs1 = reg.read(args.rs1);
                    auto dst = rs1 + args.imm;
                    reg.write_pc(dst);
                }
            ),
            Inst<S, XLEN>(
                "jal",
                0b1101111,
                0x0, // invalid
                0x0, // invalid
                ImmType::J,
                [](Reg<S, XLEN>& reg, const Args args) {
                    reg.write(args.rd, reg.read_pc() + 1);

                    auto dst = args.imm + reg.read_pc();
                    reg.write_pc(dst);
                }
            ),
            // Load Immediate->Upper
            Inst<S, XLEN>(
                "auipc",
                0b0010111,
                0x0, // invalid
                0x0, // invalid
                ImmType::U,
                [](Reg<S, XLEN>& reg, const Args args) {
                    auto dst = args.imm + reg.read_pc();
                    reg.write(args.rd, dst);
                    reg.incr_pc();
                }
            ),
            Inst<S, XLEN>(
                "lui",
                0b0110111,
                0x0, // invalid
                0x0, // invalid
                ImmType::U,
                [](Reg<S, XLEN>& reg, const Args args) {
                    reg.write(args.rd, args.imm);
                    reg.incr_pc();
                }
            ),

        };
    }
};