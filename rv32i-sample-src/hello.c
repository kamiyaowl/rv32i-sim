
#include <stdint.h>

#define UART_PERIPHERAL_BASE_ADDR (0x10000000)

void uart_tx(const char c) {
    volatile uint8_t* uartTxPtr = (volatile uint8_t*)UART_PERIPHERAL_BASE_ADDR;
    // TODO: もし実機を完全に模倣するなら送信バッファFullフラグで待ったりする
    // TxBufに値を書き込み
    *uartTxPtr = (uint8_t)c;
}
void print(const char* str) {
    for(int i = 0 ; str[i] != '\0' ; ++i) {
        uart_tx(str[i]);
    }
}
int main(void) {
    const char* hello = "Hello RISC-V! \n";
    print(hello);

    return 0;
}