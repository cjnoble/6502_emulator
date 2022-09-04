#include <stdio.h>

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned int u32;

#define MAX_MEM 0xFFFF
//static const MAX_MEM = 0xFFFF;

typedef struct Mem
{
    Byte Data[MAX_MEM];
}Mem;


typedef struct CPU
{
    Word PC; //Prgram Counter
    Word SP; //Stack Pointer

    Byte A, X, Y; // Registers

    // Status Flags
    Byte C : 1; // Carry
    Byte Z : 1; // Zero
    Byte I : 1; // Interupt
    Byte D : 1; // Decimal Mode
    Byte B : 1; // Break
    Byte V : 1; // Overflow
    Byte N : 1; // Negatove

}CPU;


void initalise_memory (Mem *mem)
{
    for (u32 i = 0; i<MAX_MEM; i++)
    {
        mem->Data[i] = 0;
    }
}

void Reset (CPU *cpu, Mem *mem)
{
    cpu->PC = 0xFFFC; // reset vector
    cpu->SP = 0x0100; //

    cpu->D = 0;
    cpu->C = cpu->Z = cpu->I = cpu->B = cpu->V = cpu->N = cpu->C = 0;

    cpu->A = cpu->X = cpu->Y = 0;

    initalise_memory(mem);

}

Byte ReadByte (CPU* cpu, Mem* mem, u32* cycles, Byte address)
{
    Byte data = mem->Data[address];
    (*cycles)--;
    return data;
}

void WriteByte (CPU* cpu, Mem* mem, u32* cycles, Byte byte, Word address)
{
    mem->Data[address] = byte;
    cycles --;
    return;
}

void WriteWord (CPU* cpu, Mem* mem, u32* cycles, Word word, Word address)
{
    mem->Data[address] = 0x00FF & word;
    address ++;
    cycles --;
    mem->Data[address] = (word >> 8);
    cycles --;
    return;
}

Byte FetchByte (CPU* cpu, Mem* mem, u32* cycles)
{
    Byte data = mem->Data[cpu->PC];
    (*cycles)--;
    cpu->PC ++;

    return data;
}

Word FetchWord (CPU* cpu, Mem* mem, u32* cycles)
{
    // Little Endian
    Word data = FetchByte(cpu, mem, cycles); // Least Signficant
    data |= (FetchByte(cpu, mem, cycles) << 8); // Most Significant
    return data;
}


void LDASetStatus(CPU *cpu)
{
    cpu-> Z = (cpu-> A == 0);
    cpu -> N = (cpu-> A & 0b10000000) > 0; //bit mask for 7th bit
}


// opcodes
#define INS_LDA_IM 0xA9
#define INS_LDA_ZP 0xA5
#define INS_LDA_ZPX 0xB5
#define INS_JSR 0x20


void Execute (CPU* cpu, Mem* mem, u32 cycles)
{
    while (cycles > 0)
    {
        Byte instruction = FetchByte(cpu, mem, &cycles);
    
        switch (instruction)
        {
            case INS_LDA_IM:
            {
                Byte value = FetchByte(cpu, mem, &cycles);
                cpu-> A = value;
                LDASetStatus(cpu);
                
            }
            break;
            
            case INS_LDA_ZP:
            {
                Byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
                cpu-> A = ReadByte(cpu, mem, &cycles, ZeroPageAddress);
                LDASetStatus(cpu);
            }
            break;

            case INS_LDA_ZPX:
            {
                Byte ZeroPageAddress = FetchByte(cpu, mem, &cycles);
                ZeroPageAddress  += cpu-> X; 
                cycles--;
                cpu-> A = ReadByte(cpu, mem, &cycles, ZeroPageAddress);
                LDASetStatus(cpu);
            }
            break;

            case INS_JSR:
            {
                Word SubAddress = FetchWord(cpu, mem, &cycles);
                // Push return point to stack
                WriteWord(cpu, mem, &cycles, cpu->PC - 1, cpu->SP);                
                cpu->SP += 2;

                cpu -> PC = SubAddress;
                cycles--;
            }
            break;

            default:
                printf("Instruction %d not handeled \n", instruction);
        }
        printf("A: %d \n", cpu-> A);
    }
}

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