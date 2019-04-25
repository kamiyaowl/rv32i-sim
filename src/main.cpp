#include <iostream>
#include <stdint.h>

#include "Inst.h"
#include "Reg.h"

using T = uint32_t;
const size_t XLEN = 32;

int main(void) {
    std::cout << "Hello" << std::endl;
    sim::Inst<T, XLEN> add("add", 0b0110011, sim::ImmType::I, [](sim::Reg<T, XLEN>& reg, const sim::Args args){});

    return 0;
}