#include <string>
#include <stdint.h>
#include <functional>
#include <bitset>

using namespace std;

namespace sim {
    // TODO: 命令エンコーディングによってかっこよく分解して
    enum ImmType: char {
        R='R', I='I', S='S', U='U',
    };
    struct Args {
        uint8_t opcode;
        uint8_t rs1;
        uint8_t rs2;
    };

    template <typename T>
    class Inst {
        protected:
            string name;
            uint32_t opcode;
            ImmType immType;
            function<
    };
};