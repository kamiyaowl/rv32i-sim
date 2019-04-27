#include <vector>
#include <string>
#pragma once

#include <stdint.h>

using namespace std;

namespace sim {
    template<typename T, size_t XLEN>
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
            Reg() {
                x = vector<T>(XLEN);
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
                this->pc++;
            }
    };
};