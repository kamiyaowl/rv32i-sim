#pragma once

#include <stdio.h>
#include <vector>

namespace sim {
    namespace log {
        enum Level: int {
            Debug = 0, 
            Info = 1, 
            Warn = 2,
            Error = 5, 
            Uart = 100,
        };
        // 出力レベルの設定
        Level print_level = Level::Info;
        // Uart出力は持っておく
        std::vector<char> uart_tx_log;

        template <typename ... T>
        void print(const char *format, T const & ... args) {
            printf(format, args ...);
        }
        template <typename ... T>
        void debug(const char *format, T const & ... args) {
            if (print_level > Level::Debug) return;
            printf("[DEBUG]");
            print(format, args ...);
        }
        template <typename ... T>
        void info(const char *format, T const & ... args) {
            if (print_level > Level::Info) return;
            printf("[INFO]");
            print(format, args ...);
        }
        template <typename ... T>
        void warn(const char *format, T const & ... args) {
            if (print_level > Level::Warn) return;
            printf("[WARN]");
            print(format, args ...);
        }
        template <typename ... T>
        void error(const char *format, T const & ... args) {
            if (print_level > Level::Error) return;
            printf("[ERROR]");
            print(format, args ...);
        }
        void uart(char c) {
            printf("\x1b[43m%c\x1b[49m", c);
            uart_tx_log.push_back(c);
        }
        void uart_log_print() {
            printf("[UART][LOG_PRINT]\n");
            for(const char c: uart_tx_log) {
                printf("%c", c);
            }
            printf("\n");
        }
        void uart_clear_log() {
            uart_tx_log.clear();
        }
    };
};