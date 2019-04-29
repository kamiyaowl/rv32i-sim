#pragma once
#include <functional>
#include "Reg.h"
#include "Inst.h"

namespace sim {
    // Register-OP
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_r_inst(string name, uint8_t funct3, uint8_t funct7, function<DATA(DATA, ADDR)> p) {
        return Inst<DATA, ADDR>(
            name,
            0b0110011,
            funct3,
            funct7,
            ImmType::R,
            [&](Reg<DATA, ADDR>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                auto r1 = reg.read(args.rs1);
                auto r2 = reg.read(args.rs2);
                auto dst = p(r1, r2);
                reg.write(args.rd, dst);
                reg.incr_pc();
            }
        );
    }
    // Immediate-OP
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_i_inst(string name, uint8_t funct3, uint8_t funct7, function<DATA(DATA, ADDR)> p) {
        return Inst<DATA, ADDR>(
            name,
            0b0010011,
            funct3,
            funct7, // invalid(srai, srliの見分けるためだけに必要、通常は0x0)
            ImmType::I,
            [&](Reg<DATA, ADDR>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                auto r1 = reg.read(args.rs1);
                auto imm = args.imm;
                auto dst = p(r1, imm);
                reg.write(args.rd, dst);
                reg.incr_pc();
            }
        );
    }
    // Store
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_s_inst(string name, uint8_t funct3, function<DATA(DATA)> p) {
        return Inst<DATA, ADDR>(
            name,
            0b0100011,
            funct3,
            0x0, // invalid
            ImmType::S,
            [&](Reg<DATA, ADDR>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                ADDR addr = reg.read(args.rs1) + args.imm;
                auto data = p(reg.read(args.rs2));

                mem.write(addr, data);
                reg.incr_pc();
            }
        );
    }
    // Load
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_l_inst(string name, uint8_t funct3, function<DATA(DATA)> p) {
        return Inst<DATA, ADDR>(
            name,
            0b0000011,
            funct3,
            0x0, // invalid
            ImmType::I,
            [&](Reg<DATA, ADDR>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                ADDR addr = reg.read(args.rs1) + args.imm;
                auto data = mem.read(addr);

                auto dst = p(data);
                reg.write(args.rd, dst);
                reg.incr_pc();
            }
        );
    }
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_b_inst(string name, uint8_t funct3, function<bool(DATA,ADDR)> cond) {
        return Inst<DATA, ADDR>(
            name,
            0b1100011,
            funct3,
            0x0, // invalid
            ImmType::B,
            [&](Reg<DATA, ADDR>& reg, Mem<DATA, ADDR>& mem, const Args args) {
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
        using S    = int32_t;
        using U    = uint32_t;
        using ADDR = uint32_t;
        const vector<Inst<S, ADDR>> instructions = {
            // OP-Reg
            alu_32i_r_inst<S, ADDR>("add"   , 0b000, 0b0000000, [](S a, S b) { return a + b; }),
            alu_32i_r_inst<S, ADDR>("sub"   , 0b000, 0b0100000, [](S a, S b) { return a - b; }),
            alu_32i_r_inst<S, ADDR>("sll"   , 0b001, 0b0000000, [](S a, S b) { assert(b > -1); return static_cast<U>(a) << b; }),
            alu_32i_r_inst<S, ADDR>("slt"   , 0b010, 0b0000000, [](S a, S b) { return a < b ? 0x1 : 0x0; }),
            alu_32i_r_inst<S, ADDR>("sltu"  , 0b011, 0b0000000, [](S a, S b) { return static_cast<U>(a) < static_cast<U>(b) ? 0x1 : 0x0; }),
            alu_32i_r_inst<S, ADDR>("xor"   , 0b100, 0b0000000, [](S a, S b) { return static_cast<U>(a) ^ static_cast<U>(b); }),
            alu_32i_r_inst<S, ADDR>("srl"   , 0b101, 0b0000000, [](S a, S b) { assert(b > -1); return static_cast<U>(a) >> b; }),
            alu_32i_r_inst<S, ADDR>("sra"   , 0b101, 0b0100000, [](S a, S b) { assert(b > -1); return a >> b; }),
            alu_32i_r_inst<S, ADDR>("or"    , 0b110, 0b0000000, [](S a, S b) { return static_cast<U>(a) | static_cast<U>(b); }),
            alu_32i_r_inst<S, ADDR>("and"   , 0b111, 0b0000000, [](S a, S b) { return static_cast<U>(a) & static_cast<U>(b); }),
            // OP-IMM
            alu_32i_i_inst<S, ADDR>("slli"  , 0b001, 0b0000000, [](S a, S imm) { assert((imm & 0xfe0) == 0x000); assert(imm > -1); return static_cast<U>(a) << (imm & 0x1f); }), // shamt==imm[24:20]
            alu_32i_i_inst<S, ADDR>("srli"  , 0b101, 0b0000000, [](S a, S imm) { assert((imm & 0xfe0) == 0x000); assert(imm > -1); return static_cast<U>(a) >> (imm & 0x1f); }), // shamt==imm[24:20]
            alu_32i_i_inst<S, ADDR>("srai"  , 0b101, 0b0100000, [](S a, S imm) { assert((imm & 0xfe0) == 0x400); assert(imm > -1); return a >> (imm & 0x1f); }), // shamt==imm[24:20]
            alu_32i_i_inst<S, ADDR>("addi"  , 0b000, 0b0000000, [](S a, S imm) { return a + imm; }),
            alu_32i_i_inst<S, ADDR>("slti"  , 0b010, 0b0000000, [](S a, S imm) { return a < imm ? 0x1 : 0x0; }),
            alu_32i_i_inst<S, ADDR>("sltiu" , 0b011, 0b0000000, [](S a, S imm) { return static_cast<U>(a) < static_cast<U>(imm) ? 0x1 : 0x0; }),
            alu_32i_i_inst<S, ADDR>("xori"  , 0b100, 0b0000000, [](S a, S imm) { return static_cast<U>(a) ^ static_cast<U>(imm); }),
            alu_32i_i_inst<S, ADDR>("ori"   , 0b110, 0b0000000, [](S a, S imm) { return static_cast<U>(a) | static_cast<U>(imm); }),
            alu_32i_i_inst<S, ADDR>("andi"  , 0b111, 0b0000000, [](S a, S imm) { return static_cast<U>(a) & static_cast<U>(imm); }),
            // Store
            alu_32i_s_inst<S, ADDR>("sb"    , 0b000, [](S a) { return static_cast<S>(static_cast<U>(a) & 0xff); }),
            alu_32i_s_inst<S, ADDR>("sh"    , 0b001, [](S a) { return static_cast<S>(static_cast<U>(a) & 0xffff); }),
            alu_32i_s_inst<S, ADDR>("sw"    , 0b010, [](S a) { return static_cast<S>(static_cast<U>(a) & 0xffffffff); }),
            // Load
            alu_32i_l_inst<S, ADDR>("lb"    , 0b000, [](S a) { return static_cast<S>(static_cast<int8_t>((static_cast<U>(a) & 0xff))); }), // 1byte符号拡張
            alu_32i_l_inst<S, ADDR>("lh"    , 0b001, [](S a) { return static_cast<S>(static_cast<int16_t>((static_cast<U>(a) & 0xffff))); }), // 2byte符号拡張
            alu_32i_l_inst<S, ADDR>("lw"    , 0b010, [](S a) { return static_cast<S>(static_cast<int32_t>((static_cast<U>(a) & 0xffffffff))); }), // 4byte符号拡張
            alu_32i_l_inst<S, ADDR>("lbu"   , 0b100, [](S a) { return static_cast<S>(static_cast<uint16_t>((static_cast<U>(a) & 0xff))); }), // 1byte Zero拡張
            alu_32i_l_inst<S, ADDR>("lhu"   , 0b101, [](S a) { return static_cast<S>(static_cast<uint32_t>((static_cast<U>(a) & 0xffff))); }), // 2byte Zero拡張
            alu_32i_l_inst<S, ADDR>("lwu"   , 0b110, [](S a) { return static_cast<S>(static_cast<uint64_t>((static_cast<U>(a) & 0xffffffffL))); }), // 4byte Zero拡張
            // Branch
            alu_32i_b_inst<S, ADDR>("beq"   , 0b000, [](S a, S b) { return a == b; }),
            alu_32i_b_inst<S, ADDR>("bne"   , 0b001, [](S a, S b) { return a != b; }),
            alu_32i_b_inst<S, ADDR>("blt"   , 0b100, [](S a, S b) { return a <  b; }),
            alu_32i_b_inst<S, ADDR>("bge"   , 0b101, [](S a, S b) { return a >  b; }),
            alu_32i_b_inst<S, ADDR>("bltu"  , 0b110, [](S a, S b) { return static_cast<U>(a) <   static_cast<U>(b); }),
            alu_32i_b_inst<S, ADDR>("bgeu"  , 0b111, [](S a, S b) { return static_cast<U>(a) >=  static_cast<U>(b); }),
            // Jump
            Inst<S, ADDR>(
                "jalr",
                0b1100111,
                0x0, // invalid
                0x0, // invalid
                ImmType::I,
                [](Reg<S>& reg, Mem<S, ADDR>& mem, const Args args) {
                    reg.write(args.rd, reg.read_pc() + 1);

                    auto rs1 = reg.read(args.rs1);
                    auto dst = rs1 + args.imm;
                    reg.write_pc(dst);
                }
            ),
            Inst<S, ADDR>(
                "jal",
                0b1101111,
                0x0, // invalid
                0x0, // invalid
                ImmType::J,
                [](Reg<S>& reg, Mem<S, ADDR>& mem, const Args args) {
                    reg.write(args.rd, reg.read_pc() + 1);

                    auto dst = args.imm + reg.read_pc();
                    reg.write_pc(dst);
                }
            ),
            // Load Immediate->Upper
            Inst<S, ADDR>(
                "auipc",
                0b0010111,
                0x0, // invalid
                0x0, // invalid
                ImmType::U,
                [](Reg<S>& reg, Mem<S, ADDR>& mem, const Args args) {
                    auto dst = args.imm + reg.read_pc();
                    reg.write(args.rd, dst);
                    reg.incr_pc();
                }
            ),
            Inst<S, ADDR>(
                "lui",
                0b0110111,
                0x0, // invalid
                0x0, // invalid
                ImmType::U,
                [](Reg<S>& reg, Mem<S, ADDR>& mem, const Args args) {
                    reg.write(args.rd, args.imm);
                    reg.incr_pc();
                }
            ),

        };
    }
};