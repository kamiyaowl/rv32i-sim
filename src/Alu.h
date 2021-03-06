#pragma once
#include <functional>
#include <vector>
#include <algorithm>

#include "Reg.h"
#include "Inst.h"

#include "Logger.h"

namespace sim {
    // Register-OP
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_r_inst(string name, uint8_t funct3, uint8_t funct7, function<DATA(DATA, DATA)> p) {
        return Inst<DATA, ADDR>(
            name,
            0b0110011,
            funct3,
            funct7,
            ImmType::R,
            [&](Reg<DATA>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                DATA r1 = reg.read(args.rs1);
                DATA r2 = reg.read(args.rs2);
                DATA dst = p(r1, r2);
                reg.write(args.rd, dst);
                reg.incr_pc();
            }
        );
    }
    // Immediate-OP
    template<typename DATA, typename ADDR>
    inline Inst<DATA, ADDR> alu_32i_i_inst(string name, uint8_t funct3, uint8_t funct7, function<DATA(DATA, DATA)> p) {
        return Inst<DATA, ADDR>(
            name,
            0b0010011,
            funct3,
            funct7, // invalid(srai, srliの見分けるためだけに必要、通常は0x0)
            ImmType::I,
            [&](Reg<DATA>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                DATA r1 = reg.read(args.rs1);
                DATA imm = args.imm_signed;
                DATA dst = p(r1, imm);
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
            [&](Reg<DATA>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                ADDR addr = reg.read(args.rs1) + args.imm_signed;
                DATA data = p(reg.read(args.rs2));

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
            [&](Reg<DATA>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                ADDR addr = reg.read(args.rs1) + args.imm_signed;
                DATA data = mem.read(addr);

                DATA dst = p(data);
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
            [&](Reg<DATA>& reg, Mem<DATA, ADDR>& mem, const Args args) {
                DATA r1 = reg.read(args.rs1);
                DATA r2 = reg.read(args.rs2);
                if (cond(r1, r2)) {
                    DATA dst = reg.read_pc() + args.imm_signed;
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
            alu_32i_b_inst<S, ADDR>("bge"   , 0b101, [](S a, S b) { return a >=  b; }),
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
                    reg.write(args.rd, reg.read_pc() + reg.get_pc_offset());

                    S rs1 = reg.read(args.rs1);
                    S dst = rs1 + args.imm_signed;
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
                    reg.write(args.rd, reg.read_pc() + reg.get_pc_offset());

                    S dst = args.imm_signed + reg.read_pc();
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
                    S dst = args.imm_signed + reg.read_pc();
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
                    reg.write(args.rd, args.imm_raw);
                    reg.incr_pc();
                }
            ),

        };

        template<typename DATA, typename ADDR>
        class Alu {
            protected:
            public:
            void reset() {}
            // 本当はいきなりALUがやるものでもない(i以外の命令セットはALUが処理しないのでDecoder.hがいるのが正確っぽい)
            bool run(Reg<DATA>& reg, Mem<DATA, ADDR>& mem, DATA instruction) {
                uint64_t inst = static_cast<uint64_t>(instruction);
                // opで検索
                uint8_t opcode = (inst >> 0) & 0x7f;
                vector<Inst<DATA, ADDR>> filter_op;
                std::copy_if(instructions.begin(), instructions.end(), std::back_inserter(filter_op), [&opcode](const Inst<DATA, ADDR>& i) {
                    return opcode == i.opcode;
                });
                // 命令が見つけられなかった場合
                if(filter_op.size() == 0) {
                    sim::log::error("[ALU] OP not found.(search operand)\n");
                    return false;
                }
                // functなしで確定するパターンが有る
                if (filter_op.size() == 1) {
                    filter_op[0].run(reg, mem, inst);
                    return true;
                } else {
                    // funct3で一致確認
                    uint8_t funct3 = (inst >> 12) & 0x7;
                    vector<Inst<DATA, ADDR>> filter_funct3;
                    std::copy_if(filter_op.begin(), filter_op.end(), std::back_inserter(filter_funct3), [&funct3](const Inst<DATA, ADDR>& i) {
                        return funct3 == i.funct3;
                    });
                    if(filter_funct3.size() == 0) {
                        sim::log::error("[ALU] OP not found.(search funct3)\n");
                        return false;
                    }
                    if (filter_funct3.size() == 1) {
                        filter_funct3[0].run(reg, mem, inst);
                        return true;
                    } else {
                        // funct7で分類が必要なパターン(add, addi, srli, sraiなど)
                        uint8_t funct7 = (inst >> 25) & 0x7f;
                        vector<Inst<DATA, ADDR>> filter_funct7;
                        std::copy_if(filter_funct3.begin(), filter_funct3.end(), std::back_inserter(filter_funct7), [&funct7](const Inst<DATA, ADDR>& i) {
                            return funct7 == i.funct7;
                        });
                        if(filter_funct7.size() == 0) {
                            sim::log::error("[ALU] OP not found.(search funct7)\n");
                            return false;
                        } else if (filter_funct7.size() > 1) {
                            sim::log::error("[ALU] OP not found.(search funct7 size>1)\n");
                            return false;
                        }
                        filter_funct7[0].run(reg, mem, inst);
                        return true;
                    }
                }
                // Done
            }


        };
    };
};