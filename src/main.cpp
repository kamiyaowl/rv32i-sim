#include <string>

#include "Cpu.h"
#include "Logger.h"

int main(int argc, char *argv[]) {
    sim::rv32i::Cpu cpu;
    // コマンドライン引数で読み込むプログラムを決定する
    std::string elf_path = "/Users/user/Documents/rv32i-sim/rv32i-sample-src/hello.o";
    if (argc > 1) {
        elf_path = argv[1];
    }
    sim::log::print_level = sim::log::Level::Uart;

    cpu.reset();
    cpu.load_program(elf_path);
    cpu.run();

    return 0;
}