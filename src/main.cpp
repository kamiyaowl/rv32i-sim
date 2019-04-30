#include <iostream>
#include <stdint.h>
#include <bitset>

#include "Cpu.h"

int main(void) {
    sim::rv32i::Cpu cpu;
    cpu.reset();

    // TODO: Impl here
    cpu.load_program("/Users/user/Documents/rv32i-sim/rv32i-sample-src/hello.o");

    cpu.run();

    return 0;
}