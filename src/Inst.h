#include <string>
#include <stdint.h>

using namespace std;

namespace sim {
    enum ImmType: char {
        R='R', I='I', S='S', U='U',
    };

    template <typename T>
    class Inst {
        protected:
            string name;
            uint32_t opcode;
            ImmType immType;
    };
};