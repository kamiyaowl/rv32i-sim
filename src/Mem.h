#pragma once
#include <map>

namespace sim {
    // DATA: 格納データ型, uint32_tとか
    // ADDR: アドレス空間のアクセスデータ型、通常はuint32_tとか
    template<typename DATA, typename ADDR>
    class Mem {
        private:
        public:
            T read(ADDR addr) {
                assert(false)
                return 0x0;
            }
            void write(ADDR addr, DATA data) {
                assert(false);
            }
    };
};