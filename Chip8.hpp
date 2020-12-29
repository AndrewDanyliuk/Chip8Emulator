#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>

class Chip8
{
public:
	Chip8();
	void LoadROM(char const* filename);
    uint8_t memory[4096]; //4 kB of memory
    uint8_t registers[16]{}; // stores 16 8-bit registers
    uint16_t pcRegister{}; //stores next instruction memory address
    uint16_t indexRegister{}; //stores memory addresses for operations
    uint16_t stack[16]{}; //16 level 16-bit stack
    uint8_t sp{}; //stack pointer
    uint8_t timer{}; //60 hz timer
    uint8_t soundTimer{}; //60 hz timer for sound output
    uint8_t keypad[16]{}; //16 keys, 0 through F
    uint32_t video[32 * 64]{}; //32 by 64 pixel video display
    uint16_t opcode{}; //stores the op code that maps to the opcode on a chip-8

private: 
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    void OP_00E0(); // (CLS) Clear display
};