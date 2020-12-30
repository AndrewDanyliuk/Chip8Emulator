/**
 * @file Chip8.cpp
 * @author Andrew Danyliuk
 * @brief Implementation file for a basic Chip-8 intepreter.
 * @version 0.1
 * @date 2020-12-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Chip8.hpp"


const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_ADDRESS = 0x50;
const unsigned int FIRST_TWELVE_BITS = 0xFFFU;
const unsigned int FONTSET_SIZE = 80;
const unsigned int REGISTER_VF = 15;
const unsigned int REGISTER_V0 = 0;

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
        : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    
    
    //Set initial PC address
    pcRegister = START_ADDRESS;

    //Initialize fonts into memory
    for (int i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONT_ADDRESS + i] = fontset[i];
    }

    //Generate a random int between 0 to 255 inclusive
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}

void Chip8::LoadROM(char const* filename)
{
    std::ifstream rom(filename, std::ifstream::binary | std::ios::ate);
    if (rom.is_open())
    {
        //Read contents to memory
        std::streampos size = rom.tellg();
        char* buffer = new char[size];

        //Return to start of file and fill buffer
        rom.seekg(0, std::ios_base::beg);
        rom.read(buffer, size);
        rom.close();

        // Read contents from buffer into memory
        for (long i = 0; i < size; i++)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        // Free space
        delete[] buffer;
    }
    
}

/**
 * @brief (CLS) Clear the display.
 * 
 * @effects Display by setting all entries to 0.
 * 
 */
void Chip8::OP_00E0()
{
    memset(display, 0, sizeof(display));
}

/**
 * @brief (RET) Return from a subroutine.
 * The interpreter sets the program counter to the address at the top of the stack, 
 * then subtracts 1 from the stack pointer.
 * 
 * @effects Sets pcRegister to top of stack
 */
void Chip8::OP_00EE()
{
    pcRegister = stack[sp--];
}

/**
 * @brief (JP addr) Jump to location nnn.
 * The interpreter sets the program counter to nnn.
 * 
 * @effects sets pcRegister to opcode & b.1111.1111.1111
 */
void Chip8::OP_1NNN()
{
    pcRegister = opcode & FIRST_TWELVE_BITS; //opcode & b.1111.1111.1111
}

/**
 * @brief Call subroutine at nnn.
 * The interpreter increments the stack pointer, 
 * then puts the current PC on the top of the stack. 
 * The PC is then set to nnn.
 * 
 * @effects stack by setting top of stack equal to PC
 * @effects pcRegister by equaling opcode & b.1111.1111.1111
 */
void Chip8::OP_2NNN()
{
    stack[++sp] = pcRegister;
    pcRegister = opcode & FIRST_TWELVE_BITS; //opcode & b.1111.1111.1111
}

/**
 * @brief Skip next instruction if Vx = kk.
 * The interpreter compares register Vx to kk, and if they are equal, 
 * increments the program counter by 2.
 * 
 * @effects pcRegister by incrementing by 2 if registers[Vx] == kk
 */
void Chip8::OP_3XKK()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t byte = 0x0FFU & opcode;
    if (registers[Vx] == byte)
    {
        pcRegister += 2; //skip instruction
    }
}

/**
 * @brief Skip next instruction if Vx != kk.
 * The interpreter compares register Vx to kk, and if they are not equal, 
 * increments the program counter by 2.
 * 
 * @effects pcRegister by incrementing by 2 if registers[Vx] != kk
 */
void Chip8::OP_4XKK()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t byte = 0x0FFU & opcode;
    if (registers[Vx] != byte)
    {
        pcRegister += 2; //skip instruction
    }
}

/**
 * @brief Skip next instruction if Vx = Vy.
 * The interpreter compares register Vx to register Vy, 
 * and if they are equal, increments the program counter by 2.
 * 
 * @effects pcRegister by incrementing by 2 if registers[Vx] == registers[Vy]
 */
void Chip8::OP_5XY0()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    if (Vx == Vy)
        {
            pcRegister += 2; //skip instruction
        }
}

/**
 * @brief Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 * 
 * @effects registers[Vx] by setting = kk
 */
void Chip8::OP_6XKK()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t byte = opcode & 0x00FFU;
    registers[Vx] = byte;
}

/**
 * @brief Set Vx = Vx + kk.
 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 * 
 * @effects registers[Vx] by adding kk to it
 */
void Chip8::OP_7XKK()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t byte = opcode & 0x00FFU;

    registers[Vx] += byte;
}

/**
 * @brief Set Vx = Vy.
 * Stores the value of register Vy in register Vx.
 * 
 * @effects registers[Vx] by setting equal to registers[Vy]
 */
void Chip8::OP_8XY0()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    registers[Vx] = registers[Vy];
}

/**
 * @brief Set Vx = Vx OR Vy.
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
 * A bitwise OR compares the corrseponding bits from two values, 
 * and if either bit is 1, then the same bit in the result is also 1. 
 * Otherwise, it is 0.
 * 
 * @effects registers[Vx] by ORing with registers[Vy]
 */
void Chip8::OP_8XY1()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;
    
    registers[Vx] |= registers[Vy];
}

/**
 * @brief Set Vx = Vx AND Vy.
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. 
 * A bitwise AND compares the corrseponding bits from two values, 
 * and if both bits are 1, then the same bit in the result is also 1. 
 * Otherwise, it is 0.
 * 
 * @effects registers[Vx] by ANDing with registers[Vy]
 */
void Chip8::OP_8XY2()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    registers[Vx] &= registers[Vy];
}

/**
 * @brief Set Vx = Vx XOR Vy.
 * Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. 
 * An exclusive OR compares the corrseponding bits from two values, 
 * and if the bits are not both the same, then the corresponding bit in the result is set to 1. 
 * Otherwise, it is 0.
 * 
 * @effects registers[Vx] by XORing with registers[Vy]
 */
void Chip8::OP_8XY3()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    registers[Vx] ^= registers[Vy];
}

/**
 * @brief Set Vx = Vx + Vy, set VF = carry.
 * The values of Vx and Vy are added together. 
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 * 
 * @effects registers[REGISTER_VF] by setting to 1 if registers[Vx] + registers[Vy] > 255 and 0 otherwise
 * @effects registers[Vx] by equaling sum of registers[Vx] and registers[Vy]
 */
void Chip8::OP_8XY4()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;
    
    uint16_t sum = registers[Vx] + registers[Vy];
    
    if (sum > 0x00FFU)
    {
        registers[REGISTER_VF] = 1;
    } else
    {
        registers[REGISTER_VF] = 0;
    }

    registers[Vx] = sum & 0x00FFU; 
    
}

/**
 * @brief Set Vx = Vx - Vy, set VF = NOT borrow.
 * If Vx > Vy, then VF is set to 1, otherwise 0. 
 * Then Vy is subtracted from Vx, and the results stored in Vx.
 * 
 * @effects registers[REGISTER_VF] by setting to 1 if registers[Vx] > registers[Vy] and 0 otherwise
 * @effects registers[Vx] by minusing registers[Vy] from registers[Vx]
 */
void Chip8::OP_8XY5()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    if (registers[Vx] > registers[Vy])
    {
        registers[REGISTER_VF] = 1;
    } else
    {
        registers[REGISTER_VF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

/**
 * @brief Set Vx = Vx SHR 1.
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
 * 
 * @effects registers[REGISTER_VF] by setting to the least significant bit of registers[Vx]
 * @effects registers[Vx] by shifting right by 1 bit
 */
void Chip8::OP_8XY6()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t LSB = registers[Vx] & 1;

    registers[REGISTER_VF] = LSB;

    registers[Vx] >>= 1U;
    
}

/**
 * @brief Set Vx = Vy - Vx, set VF = NOT borrow.
 * If Vy > Vx, then VF is set to 1, otherwise 0. 
 * Then Vx is subtracted from Vy, and the results stored in Vx.
 * 
 * @effects registers[REGISTER_VF] by setting to 1 if registers[Vy] > registers[Vx], 0 otherwise
 * @effects registers[Vx] by setting to registers[Vy] - registers[Vx]
 */
void Chip8::OP_8XY7()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    if (registers[Vy] > registers[Vx])
    {
        registers[REGISTER_VF] = 1;
    } else
    {
        registers[REGISTER_VF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

/**
 * @brief Set Vx = Vx SHL 1.
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. 
 * Then Vx is multiplied by 2.
 * 
 * @effects registers[REGISTER_VF] by setting to the most significant bit of registers[Vx]
 * @effects registers[Vx] by shifting left by 1 bit
 */
void Chip8::OP_8XYE()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t MSB = (registers[Vx] & 0x80U) >> 7U;

    registers[REGISTER_VF] = MSB;

    registers[Vx] <<= 1;
    
}

/**
 * @brief Skip next instruction if Vx != Vy.
 * The values of Vx and Vy are compared, and if they are not equal, 
 * the program counter is increased by 2.
 * 
 * @effects pcRegister by incrementing by 2 if registers[Vx] != registers[Vy]
 */
void Chip8::OP_9XY0()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t Vy = (0x00F0U & opcode) >> 4U;

    if (registers[Vx] != registers[Vy])
    {
        pcRegister += 2;
    }
}

/**
 * @brief Set I = nnn.
 * The value of register I is set to nnn.
 * 
 * @effects indexRegister by setting to opcode & b.1111.1111.1111
 */
void Chip8::OP_ANNN()
{
    indexRegister = opcode & 0x0FFFU;
}

/**
 * @brief Jump to location nnn + V0.
 * The program counter is set to nnn plus the value of V0.
 * 
 * @effects pcRegister by setting to registers[REGISTER_V0] + (opcode & b.1111.1111.1111)
 */
void Chip8::OP_BNNN()
{
    pcRegister = registers[REGISTER_V0] + (opcode & 0x0FFFU);
}

/**
 * @brief Set Vx = random byte AND kk.
 * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. 
 * The results are stored in Vx. See instruction OP_8XY2 for more information on AND.
 * 
 * @effects registers[Vx] by setting to kk ANDed with a random number between 0 to 255
 */
void Chip8::OP_CXKK()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    uint8_t byte = opcode & 0x00FFU;

    registers[Vx] = byte & randByte(randGen);
}

void Chip8::OP_DXYN()
{
    //TODO: add this draw instruction
}

void Chip8::OP_EX9E()
{
    //TODO: add this
}

void Chip8::OP_EXA1()
{
    //TODO: add this
}

/**
 * @brief Set Vx = delay timer value.
 * The value of DT is placed into Vx.
 * 
 * @effects registers[Vx] by setting to the value of the 60 Hz delay timer
 */
void Chip8::OP_FX07()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    
    registers[Vx] = timer;
}

void Chip8::OP_FX0A()
{
    //TODO: add this
}

/**
 * @brief Set delay timer = Vx.
 * DT is set equal to the value of Vx.
 * 
 * @effects timer by setting to registers[Vx]
 */
void Chip8::OP_FX15()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;
    
    timer = registers[Vx];
}

/**
 * @brief Set sound timer = Vx.
 * ST is set equal to the value of Vx.
 * 
 * @effects soundTimer by setting equal to registers[Vx]
 */
void Chip8::OP_FX18()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;

    soundTimer = registers[Vx];
}

/**
 * @brief Set I = I + Vx.
 * The values of I and Vx are added, and the results are stored in I.
 * 
 * @effects indexRegister by adding registers[Vx]
 */
void Chip8::OP_FX1E()
{
    uint8_t Vx = (0x0F00U & opcode) >> 8U;

    indexRegister += registers[Vx]
}

void Chip8::OP_FX29()
{
    //TODO: add this
}

void Chip8::OP_FX33()
{
    //TODO: add this
}

void Chip8::OP_FX55()
{
    //TODO: add this
}

void Chip8::OP_FX65()
{
    //TODO: add this
}