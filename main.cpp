#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <ios>
#include <cstdio>
#include <cstdint>
#include <utility>
#include <fstream>

int main(int argc, char const** argv) {
    std::uint8_t *memory = new std::uint8_t[4096];
    bool *display = new bool[2048];
    bool *keystates = new bool[16];
    std::uint16_t stack[16];
    std::uint8_t registers[16];
    
    
    std::memset(memory, 0, 4096);
    std::memset(display, 0, 2048);
    std::memset(stack, 0, 32);
    std::memset(registers, 0, 16);
    std::memset(keystates, 0, 16);
    // FONTS
    
    //0
    memory[0] = 0xF0;
    memory[1] = 0x90;
    memory[2] = 0x90;
    memory[3] = 0x90;
    memory[4] = 0xF0;
    //1
    memory[5] = 0x20;
    memory[6] = 0x60;
    memory[7] = 0x20;
    memory[8] = 0x20;
    memory[9] = 0x70;
    //2
    memory[10] = 0xF0;
    memory[11] = 0x10;
    memory[12] = 0xF0;
    memory[13] = 0x80;
    memory[14] = 0xF0;
    //3
    memory[15] = 0xF0;
    memory[16] = 0x10;
    memory[17] = 0xF0;
    memory[18] = 0x10;
    memory[19] = 0xF0;
    //4
    memory[20] = 0x90;
    memory[21] = 0x90;
    memory[22] = 0xF0;
    memory[23] = 0x10;
    memory[24] = 0x10;
    //5
    memory[25] = 0xF0;
    memory[26] = 0x80;
    memory[27] = 0xF0;
    memory[28] = 0x10;
    memory[29] = 0xF0;
    //6
    memory[30] = 0xF0;
    memory[31] = 0x80;
    memory[32] = 0xF0;
    memory[33] = 0x90;
    memory[34] = 0xF0;
    //7
    memory[35] = 0xF0;
    memory[36] = 0x10;
    memory[37] = 0x20;
    memory[38] = 0x40;
    memory[39] = 0x40;
    //8
    memory[40] = 0xF0;
    memory[41] = 0x90;
    memory[42] = 0xF0;
    memory[43] = 0x90;
    memory[44] = 0xF0;
    //9
    memory[45] = 0xF0;
    memory[46] = 0x90;
    memory[47] = 0xF0;
    memory[48] = 0x10;
    memory[49] = 0xF0;
    //A
    memory[50] = 0xF0;
    memory[51] = 0x90;
    memory[52] = 0xF0;
    memory[53] = 0x90;
    memory[54] = 0x90;
    //B
    memory[55] = 0xE0;
    memory[56] = 0x90;
    memory[57] = 0xE0;
    memory[58] = 0x90;
    memory[59] = 0xE0;
    //C
    memory[60] = 0xF0;
    memory[61] = 0x80;
    memory[62] = 0x80;
    memory[63] = 0x80;
    memory[64] = 0xF0;
    //D
    memory[65] = 0xE0;
    memory[66] = 0x90;
    memory[67] = 0x90;
    memory[68] = 0x90;
    memory[69] = 0xE0;
    //E
    memory[70] = 0xF0;
    memory[71] = 0x80;
    memory[72] = 0xF0;
    memory[73] = 0x80;
    memory[74] = 0xF0;
    //F
    memory[75] = 0xF0;
    memory[76] = 0x80;
    memory[77] = 0xF0;
    memory[78] = 0x80;
    memory[79] = 0x80;
    
    if (argc != 2) {
        std::cout << "Pass one argument please." << std::endl;
        return 0;
    } else {
        std::FILE *fileHandle = std::fopen(argv[1], "r");
        std::fread(&(memory[512]), 1, 3584, fileHandle);
        std::fclose(fileHandle);
    }
    
    std::uint16_t I=0;
    std::uint8_t delay=0;
    std::uint8_t sound=0;

    std::uint16_t programCounter=0x0200;
    std::uint8_t stackPointer=0;
    
    sf::Image screenPixels;
    screenPixels.create(64, 32);
    sf::Texture screenTexture;
    sf::Sprite screenSprite;
    
    sf::RenderWindow window(sf::VideoMode(640, 320), "CHIP-8");
    window.setFramerateLimit(60);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        keystates[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
        keystates[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
        keystates[2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
        keystates[3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);
        keystates[4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
        keystates[5] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        keystates[6] = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
        keystates[7] = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
        keystates[8] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        keystates[9] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        keystates[10] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        keystates[11] = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
        keystates[12] = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        keystates[13] = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
        keystates[14] = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
        keystates[15] = sf::Keyboard::isKeyPressed(sf::Keyboard::V);
        // fetch
        if (programCounter >= 4094) {
            std::cout << "CHIP-8 message: Program counter reached end of memory." << std::endl;
            std::exit(0);
        }
        std::uint16_t instruction = (memory[programCounter] << 8) + memory[programCounter+1];
        //std::cout << programCounter << std::endl;
        std::cout << std::hex << (int) instruction << std::endl;
        programCounter=programCounter+2;
        // execute
        if (instruction==0x00E0) {  // 00E0 - CLS
            std::memset(display,0,2048);
        }
        else if (instruction==0x00EE) { // 00EE - RET
            programCounter=stack[(stackPointer-1)%16];
            stackPointer--;
        }
        else if ((instruction & 0xF000) == 0x1000) { // 1nnn - JP addr
            programCounter=instruction & 0x0FFF;
        }
        else if ((instruction & 0xF000) == 0x2000) { // 2nnn - CALL addr
            stackPointer++;
            stack[(stackPointer-1)%16] = programCounter;
            programCounter = instruction & 0x0FFF;
        }
        else if ((instruction & 0xF000) == 0x3000) { // 3xkk - SE Vx, byte
            if (registers[(instruction & 0x0F00)>>8] == (instruction & 0x00FF)) {
                programCounter+=2;
            }
        }
        else if ((instruction & 0xF000) == 0x4000) { // 4xkk - SNE Vx, byte
            if (registers[(instruction & 0x0F00)>>8] == (instruction & 0x00FF)) {
            } else {
                programCounter+=2;
            }
        }
        else if (((instruction & 0xF000)==0x5000) && ((instruction & 0x000F) == 0x0000)) { //5xy0 - SE Vx, VY
            if (registers[(instruction & 0x0F00)>>8] == registers[(instruction & 0x00F0)>>4]) {
                programCounter+=2;
            }
        } else if ((instruction & 0xF000)==0x6000) { // 6XNN - LD Vx, byte
            registers[(instruction & 0x0F00)>>8] = (instruction & 0x00FF);
        }
        else if ((instruction & 0xF000)==0x7000) { // 7XNN - ADD Vx, byte
            registers[(instruction & 0x0F00)>>8] += instruction & 0x00FF;
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0000)) { // 8xy0 - LD Vx, Vy
            registers[(instruction & 0x0F00)>>8] = registers[(instruction & 0x00F0)>>4];
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0001)) { // 8xy1 - OR Vx, Vy
            registers[(instruction & 0x0F00)>>8] |= registers[(instruction & 0x00F0)>>4];
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0002)) { // 8xy2 - AND Vx, Vy
            registers[(instruction & 0x0F00)>>8] &= registers[(instruction & 0x00F0)>>4];
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0003)) { // 8xy3 - XOR Vx, Vy
            registers[(instruction & 0x0F00)>>8] ^= registers[(instruction & 0x00F0)>>4];
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0004)) { // 8xy4 - ADD Vx, Vy (set VF)
            if ((int) registers[(instruction & 0x0F00)>>8] + (int) registers[(instruction & 0x00F0)>>4] > 256) {
                registers[15] = 1;
            } else {
                registers[15] = 0;
            }
            registers[(instruction&0x0F00)>>8] += registers[(instruction&0x00F0)>>4];
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0005)) { // 8xy5 - SUB Vx, Vy (set VF)
            if ((int) registers[(instruction & 0x0F00)>>8] - (int) registers[(instruction & 0x00F0)>>4] >= 0) {
                registers[15] = 1;
            } else {
                registers[15] = 0;
            }
            registers[(instruction&0x0F00)>>8] -= registers[(instruction&0x00F0)>>4];
        }
        else if ((instruction & 0xF00F)==0x8006) { // 8xy6 - SHR Vx { ,Vy} (set VF)
            registers[15] = registers[(instruction&0x0F00)>>8]&1;
            registers[(instruction&0x0F00)>>8] >>= 1;
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x0007)) { // 8xy7 - SUBN Vx, Vy
            registers[15] = registers[(instruction&0x0F00)>>8] < registers[(instruction&0x00F0)>>4];
            registers[(instruction&0x0F00)>>8] = registers[(instruction&0x00F0)>>4] - registers[(instruction&0x0F00)>>8];
        }
        else if (((instruction & 0xF000)==0x8000) && ((instruction & 0x000F) == 0x000E)) { // 8xyE - SHL Vx {, VY}
            registers[15] = (registers[(instruction&0x0F00)>>8]&0x80)>>7;
            registers[(instruction&0x0F00)>>8] = registers[(instruction&0x00F0)>>4] << 2;
        }
        else if (((instruction & 0xF000)==0x9000)&&((instruction & 0x000F)==0x0000)) { //9xy0 - SNE Vx, Vy
            if (registers[(instruction&0x0F00)>>8] == registers[(instruction&0x00F0)>>4]) {
            } else {
                programCounter+=2;
            }
        }
        else if ((instruction & 0xF000) == 0xA000) { // Annn - LD I, addr
            I = instruction & 0x0FFF;
        }
        else if ((instruction & 0xF000) == 0xB000) { // Bnnn - JP V0, addr
            programCounter = (std::uint16_t) (instruction & 0x0FFF) + registers[0];
        }
        else if ((instruction & 0xF000) == 0xC000) { // Cxkk - RND Vx, byte
            registers[(instruction & 0x0F00)>>8] = (std::rand()%256) & (instruction & 0x00FF);
        }
        else if ((instruction & 0xF000) == 0xD000) { // Dxyn - DRW Vx, Vy, nibble
            registers[15] = 0;
            for (int y = 0; y < (instruction & 0x000F);y++) {
                for (int x = 0; x < 8;x++) {
                    if (memory[I+y] & (128 >> x)) {
                        if ( (registers[((instruction & 0x0F00)>>8)]+x) >= 0 &&  (registers[((instruction & 0x0F00)>>8)]+x) <= 64 && (registers[((instruction & 0x00F0)>>4)]+y) >= 0 && (registers[((instruction & 0x00F0)>>4)]+y) <= 32) {
                            if (display[registers[((instruction & 0x0F00)>>8)]+x +(registers[((instruction & 0x00F0)>>4)]+y)*64]) {
                                registers[15] = 1;
                            }
                            display[registers[((instruction & 0x0F00)>>8)]+x +(registers[((instruction & 0x00F0)>>4)]+y)*64] ^= 1;

                        }
                    }
                }
            }
        }
        else if ( ((instruction & 0xF000) == 0xE000) && ((instruction & 0x00F0) == 0x0090) && ((instruction & 0x000F) == 0x000E)) {
            if (keystates[registers[(instruction & 0x0F00)>>8]]) {
                programCounter+=2;
            }
        }
        else if ( ((instruction & 0xF000) == 0xE000) && ((instruction & 0x00F0) == 0x00A0) && ((instruction & 0x000F) == 0x0001)) {
            if (!(keystates[registers[(instruction & 0x0F00)>>8]])) {
                programCounter+=2;
            }
        }
        else if ((instruction & 0xF0FF)==0xF007) {
            registers[(instruction & 0x0F00)>>8] = delay;
        }
        else if ((instruction & 0xF0FF)==0xF00A) {
            int val = false;
            for (int i = 0; i < 16;i++) {
                if(keystates[i] == true) {
                    val = true;
                    registers[(instruction & 0x0F00)>>8]=i;
                    break;
                }
            }
            if (!val) {
                programCounter-=2;
            }
        }
        else if ((instruction & 0xF0FF)==0xF015) {
            delay = registers[(instruction & 0x0F00)>>8];
        }
        else if ((instruction & 0xF0FF)==0xF018) {
            sound = registers[(instruction & 0x0F00)>>8];
        }
        else if ((instruction & 0xF0FF)==0xF01E) {
            I += registers[(instruction & 0x0F00)>>8];
        }
        else if ((instruction & 0xF0FF)==0xF029) {
            I = ((registers[(instruction & 0x0F00)>>8] & 0x000F)*5)-1;
        }
        else if ((instruction & 0xF0FF)==0xF033) {
            memory[I] = registers[(instruction & 0x0F00)>>8]/100;
            memory[I+1] = (registers[(instruction & 0x0F00)>>8]/10)%10;
            memory[I+2] = (registers[(instruction & 0x0F00)>>8])%10;
        }
        else if ((instruction & 0xF0FF)==0xF055) {
            for (int i = 0; i <15;i++) {
                memory[I+i] = registers[i];
            }
        }
        else if ((instruction & 0xF0FF)==0xF065) {
            for (int i = 0; i < 15;i++) {
                registers[i] = memory[I+i];
            }
        }
        //draw
        for (int i = 0; i < 2048;i++) {
            screenPixels.setPixel(i%64, i/64, sf::Color(display[i]*255,display[i]*255,display[i]*255));
        }
        screenTexture.loadFromImage(screenPixels);
        screenSprite.setTexture(screenTexture);
        screenSprite.setScale(10, 10);
        window.draw(screenSprite);
        window.display();
    }

    return 0;
}
