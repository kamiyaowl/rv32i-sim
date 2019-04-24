#include <vector>
#include <tuple>
#include <string>
#include <stdint.h>

using namespace std;

namespace sim {
    template<typename T> // uint16_t/uint32_t/uint64_t ...
    class Reg {
        protected:
            const size_t REG_N = 32;
            vector<T> x;
            T pc;
        public:
            Reg() {
                x = vector(REG_N);
                pc = 0;
            }
            T read(uint8_t addr) {
                assert(addr < REG_N);

                if(addr == 0) return 0x0; // x0/zero
                else x[addr];
            }
            tuple<T,T> read(uint8_t addr1, uint8_t addr2) {
                return make_tuple(this->read(addr1), this->read(addr2));
            }
            void write(uint8_t addr, T data) {
                assert(0 < addr);
                assert(addr < REG_N);
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