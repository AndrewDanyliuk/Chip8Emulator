#include "Chip8.hpp"

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

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


