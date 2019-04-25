#include <iostream>
#include <stdint.h>
#include <bitset>

#include "Inst.h"
#include "Reg.h"
#include "RV32I.h"

int main(void) {
    for(const auto& pair: sim::rv32i::instructions) {
        std::cout << std::bitset<7>(pair.first) << ": [" << pair.second.name << "]" << std::endl;
    }
    return 0;
}