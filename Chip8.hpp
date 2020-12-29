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

    // CHIP-8 Instructions
    //void OP_0NNN(); // (SYS addr) Jump to a machine code routine at nnn (Ignored on modern interps)
    void OP_00E0(); // (CLS) Clear display
    void OP_00EE(); // (RET) Return from a subroutine
    void OP_1NNN(); // (JP addr) Sets PC to nnn
    void OP_2NNN(); // (CALL addr) Call subroutine at nnn
    void OP_3XKK(); // (SE Vx, byte) Skip next instruction if Vx = kk
    void OP_4XKK(); // (SNE Vx, byte) Skip next instruction if Vx != kk
    void OP_5XY0(); // (SE Vx, Vy) Skip next instruction if Vx = Vy
    void OP_6XKK(); // (LD Vx, byte) Set Vx = kk
    void OP_7XKK(); // (ADD Vx, byte) Set Vx = Vx + kk
    void OP_8XY0(); // (LD Vx, Vy) Set Vx = Vy
    void OP_8XY1(); // (OR Vx, Vy) Set Vx = Vx OR Vy
    void OP_8XY2(); // (AND Vx, Vy) Set Vx = Vx AND Vy
    void OP_8XY3(); // (XOR Vx, Vy) Set Vx = Vx XOR Vy
    void OP_8XY4(); // (ADD Vx, Vy) Set Vx = Vx + Vy, set VF = carry
    void OP_8XY5(); // (SUB Vx, Vy) Set Vx = Vx - Vy, set VF = NOT borrow
    void OP_8XY6(); // (SHR Vx, Vy) Set Vx = Vx SHR 1
    void OP_8XY7(); // (SUBN Vx, Vy) Set Vx = Vy - Vx, set VF = NOT borrow
    void OP_8XYE(); // (SHL Vx, Vy) Set Vx = Vx SHL 1
    void OP_9XY0(); // (SNE Vx, Vy) Skip next instruction if Vx != Vy
    void OP_ANNN(); // (LD I, addr) Set I = nnn
    void OP_BNNN(); // (JP V0, addr) Jump to location nnn + V0
    void OP_CXKK(); // (RND Vx, byte) Set Vx = random byte AND kk
    void OP_DXYN(); // (DRW Vx, Vy, nibble) Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    void OP_EX9E(); // (SKP Vx) Skip next instruction if key with value of Vx is pressed
    void OP_EXA1(); // (SKNP Vx) Skip next instruction if key with value of Vx is not pressed
    void OP_FX07(); // (LD Vx, DT) Set Vx = delay timer value
    void OP_FX0A(); // (LD Vx, K) Wait for a key press, store the value of the key in Vx
    void OP_FX15(); // (LD Dt, Vx) Set delay timer = Vx
    void OP_FX18(); // (LD ST, Vx) Set sound timer = Vx
    void OP_FX1E(); // (ADD I, Vx) Set I = I + Vx
    void OP_FX29(); // (LD F, Vx) Set I = location of sprite for digit Vx
    void OP_FX33(); // (LD B, Vx) Store BCD representation of Vx in memory locations I, I+1, and I+2
    void OP_FX55(); // (LD [I], Vx) Store registers V0 through Vx in memory starting at location I
    void OP_FX65(); // (LD Vx, [I]) Read registers V0 through Vx from memory starting at location I

};