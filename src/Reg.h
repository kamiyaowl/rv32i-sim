#pragma once

#include <vector>
#include <string>
#include <stdint.h>

using namespace std;

namespace sim {
    // Reg(32i用)
    template<typename T>
    class Reg {
        protected:
            // Calling conversion
            // x0     : zero  : always zero
            // x1     : ra    : return address
            // x2     : sp    : stack pointer
            // x3     : gp    : global pointer
            // x4     : tp    : thread pointer
            // x5-7   : t0-2  : temporary register
            // x8     : s0/fp : saved register / frame pointer
            // x9     : s1    : saved register
            // x10-11 : a0-1  : function arguments / return value
            // x12-17 : a2-7  : function arguments
            // x18-27 : s2-11 : saved register
            // x28-31 : t3-6  : temporary registers
            vector<T> x;
            T pc;
        public:
            const size_t XLEN = 32;
            Reg() {
                this->reset();
            }
            void reset() {
                x = vector<T>(XLEN, 0x0);
                pc = 0;
            }
            T read(uint8_t addr) {
                assert(addr < XLEN);

                if(addr == 0) return 0x0; // x0/zero
                else return x[addr];
            }
            void write(uint8_t addr, T data) {
                assert(0 < addr);
                assert(addr < XLEN);
                x[addr] = data;
            }
            T read_pc() {
                return pc;
            }
            void write_pc(T data) {
                this->pc = data;
            }
            void incr_pc() {
                this->pc += (XLEN / 8);
            }
            void debug_print() {
                printf("\n");
                for(int j = 0 ; j < XLEN / 8 ; ++j) {
                    printf("[MEM] ");
                    for(int i = 0 ; i < 8 ; ++i) {
                        int ptr = i + j * 8;
                        printf("x[%2x]:%08x ", ptr, this->x[ptr]);
                    }
                    printf("\n");
                }
                printf("[MEM] pc:%08x\n\n", this->pc);
            }
    };
};