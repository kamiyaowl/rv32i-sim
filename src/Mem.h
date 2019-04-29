#pragma once
#include <map>

namespace sim {
    // DATA: 格納データ型, uint32_tとか
    // ADDR: アドレス空間のアクセスデータ型、通常はuint32_tとか
    // Little Endian
    template<typename DATA, typename ADDR>
    class Mem {
        private:
            std::map<ADDR, uint8_t> mem;
        public:
            const ADDR UART_PERIPHERAL_BASE_ADDR = 0x10000000;
            const ADDR UART_PERIPHERAL_SIZE      = 0x00000001;
            DATA read_byte(ADDR addr) {
                if (mem.count(addr) == 0) {
                    mem[addr] = 0xa5; // 本来ランダム初期化されるので
                }
                return mem[addr];
            }
            DATA read_half(ADDR addr) {
                return (read_byte(addr + 1) << 8) | read_byte(addr);
            }
            // read_word
            DATA read(ADDR addr) {
                return (read_half(addr + 2) <<16) | read_half(addr);
            }
            // write_word
            void write_byte(ADDR addr, DATA data) {
                mem[addr] = data & 0xff;
                // UART
                if (this->UART_PERIPHERAL_BASE_ADDR <= addr && 
                    (addr < this->UART_PERIPHERAL_BASE_ADDR + this->UART_PERIPHERAL_SIZE)) {
                    // baseaddr + 0: uart_tx Queueがめっちゃあって即時送信とかにしとく
                    putchar(static_cast<char>(mem[addr]));
                }
            }
            void write_half(ADDR addr, DATA data) {
                mem[addr] = data & 0xff;
                mem[addr+1] = (data >> 8) & 0xff;
            }
            void write(ADDR addr, DATA data) {
                mem[addr] = data & 0xff;
                mem[addr+1] = (data >>  8) & 0xff;
                mem[addr+2] = (data >> 16) & 0xff;
                mem[addr+3] = (data >> 24) & 0xff;
            }
            void reset() {
                mem.clear();
            }
    };
};