#include <iostream>
#include <stdint.h>
#include <bitset>

#include "Cpu.h"

int main(void) {
    sim::rv32i::Cpu cpu;
    cpu.reset();

    // TODO: Impl here
    cpu.load_program();

    cpu.run();

    return 0;
}