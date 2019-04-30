#pragma once

#include <string>
#include <functional>
#include <fstream>

namespace sim {
    namespace elf {
        // Thanks: http://blue-9.hatenadiary.com/entry/2017/03/14/212929
        void load(const std::string& elf_path, std::function<void(uint32_t addr, uint32_t data)> write) {
            std::ifstream ifs(elf_path, std::ifstream::binary);
            assert(ifs);

            // ELF Headerを読み出す


            
            // おわり
            ifs.close();
        }
    }
}