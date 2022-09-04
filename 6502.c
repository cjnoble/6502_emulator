#include <stdio.h>
#include "6502.h"

int main()
{
    
    struct Mem mem;
    struct CPU cpu;
    Reset(&cpu, &mem);

    mem.Data[0xFFFC] = INS_JSR;
    mem.Data[0xFFFD] = 0x42;
    mem.Data[0xFFFE] = 0x42;
    mem.Data[0x4242] = INS_LDA_IM;
    mem.Data[0x4243] = 0x84;

    Execute(&cpu, &mem, 9);

    printf("%d", 0);

    return 0;
}