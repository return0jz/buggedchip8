#include <iostream>
#include <vector>
#include <memory>
#include <stack>
#include <SDL2/SDL.h>
#include <cstdint>
#include <array>

class emulator {
public:
    emulator() :
    memory(new std::array<std::uint8_t,4096>),
    display(new std::array<std::uint8_t,2048>)
    
    {
        std::fill(memory->begin(), memory->end(), 0);
        std::fill(key.begin(), key.end(),0);
        std::fill(display->begin(),display->end(),0);
        std::fill(v.begin(), v.end(), 0);
        std::array<std::uint8_t,80> fontset= {
            0xF0, 0x90, 0x90, 0x90, 0xF0,        // 0
            0x20, 0x60, 0x20, 0x20, 0x70,        // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0,        // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0,        // 3
            0x90, 0x90, 0xF0, 0x10, 0x10,        // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0,        // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0,        // 6
            0xF0, 0x10, 0x20, 0x40, 0x40,        // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0,        // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0,        // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90,        // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0,        // B
            0xF0, 0x80, 0x80, 0x80, 0xF0,        // C
            0xE0, 0x90, 0x90, 0x90, 0xE0,        // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0,        // E
            0xF0, 0x80, 0xF0, 0x80, 0x80        // F
        };
        for (int i = 0; i < 80;i++) {
            memory->at(i) = fontset[i];
        }
        I = 0;
        sound = 0;
        delay = 0;
        pc = 0x200;
    }
    void fetch() {
        if (pc > 4094) {
            pc = 0x200;
        }
        opcode = (memory->at(pc) << 8) | (memory->at(pc+1));
        //std::cout << std::hex << "pc: " << pc << " ";
        //std::cout << "opc: "<< (int) opcode << std::endl;

        x = (opcode & 0x0F00)>>8;
        y = (opcode & 0x00F0)>>4;
        nnn = opcode & 0x0FFF;
        n = opcode & 0x000F;
        kk = opcode & 0x00FF;
    }
    void execute() {
        switch (opcode & 0xF000) {
            case 0x0000:
                switch(opcode & 0x00FF) {
                    case 0x0000: // NULL instruction
                        pc+=2;
                        break;
                    case 0x00E0: // Clear screen
                        std::fill(display->begin(), display->end(), 0);
                        pc += 2;
                        break;
                    case 0x00EE: // Return from function
                        pc = stack.top()+2;
                        stack.pop();
                        break;
                }
                break;
            case 0x1000: // Jump to nnn
                pc = nnn;
                break;
            case 0x2000: // Call function
                stack.push(pc);
                pc = nnn;
                break;
            case 0x3000: // Skip if vx = byte
                if (v[x] == kk) {
                    pc+=2;
                }
                pc+=2;
                break;
            case 0x4000: // Skip if vx != byte
                if (v[x] != kk) {
                    pc+=2;
                }
                pc+=2;
                break;
            case 0x5000: // Skip if vx = vy
                if (v[x] == v[y]) {
                    pc+=2;
                }
                pc+=2;
                break;
            case 0x6000: // Load byte into vx
                v[x] = kk;
                pc+=2;
                break;
            case 0x7000: // Add byte to vx
                v[x] += kk;
                pc+=2;
                break;
            case 0x8000:
                switch (opcode & 0x000F) {
                    case 0x0000: // Load vy into vx
                        v[x]= v[y];
                        pc+=2;
                        break;
                    case 0x0001: //
                        v[x] |= v[y];
                        pc+=2;
                        break;
                    case 0x0002:
                        v[x] &= v[y];
                        pc+=2;
                        break;
                    case 0x0003:
                        v[x] ^= v[y];
                        pc+=2;
                        break;
                    case 0x0004:
                        v[0xF] = 0;
                        if ( ((int) v[x] + (int) v[y]) > 0xFF) {
                            v[0xF] = 1;
                        }
                        v[x] += v[y];
                        pc+=2;
                        break;
                    case 0x0005:
                        v[0xF] = 0;
                        if (v[x] > v[y]) {
                            v[0xF] = 1;
                        }
                        v[x] -= v[y];
                        pc+=2;
                        break;
                    case 0x0006:
                        v[0xF] = v[x] & 0x01;
                        v[x] >>= 1;
                        pc+=2;
                        break;
                    case 0x0007:
                        v[0xF] = 0;
                        if (v[x] < v[y]) {
                            v[0xF] = 1;
                        }
                        v[x] = v[y] - v[x];
                        pc+=2;
                        break;
                    case 0x000E:
                        v[0xF] = (v[x] >> 7) & 0x01;
                        v[x] <<= 1;
                        pc+=2;
                        break;
                }
                break;
            case 0x9000:
                if (v[x] != v[y]) {
                    pc+=2;
                }
                pc+=2;
                break;
            case 0xA000:
                I = nnn;
                pc+=2;
                break;
            case 0xB000:
                pc=nnn+v[0];
                break;
            case 0xC000:
                v[x] = (std::uint8_t) (std::rand() % 255) & kk;
                pc+=2;
                break;
            case 0xD000: {
                v[0xF] = 0;
                std::uint8_t vx = v[x];
                std::uint8_t vy = v[y];
                for (int i = 0; i < n;i++) {
                    std::uint8_t row = memory->at(I+i);
                    for (int p = 8; p > 0; p--) {
                        bool val = getBit(row, p);
                        std::uint8_t tx = (vx + (8-p)) %64;
                        std::uint8_t ty = (vy + i) % 32;
                        if (display->at( tx + (ty*64))) {
                            v[0xF] = 1;
                        }
                        display->at( tx + (ty*64)) ^= val;
                    }
                }
                pc+=2;
            }
                break;
            case 0xE000:
                switch (opcode & 0x00FF) {
                    case 0x009E:
                        if (key[x]) {
                            pc+=2;
                        }
                        pc+=2;
                        break;
                    case 0x00A1:
                        if (!key[x]) {
                            pc+=2;
                        }
                        pc+=2;
                        break;
                }
                break;
            case 0xF000:
                switch (opcode & 0x00FF) {
                    case 0x0007:
                        v[x] = delay;
                        pc+=2;
                        break;
                    case 0x000A:
                        for (int i = 0; i <= 0xF;i++) {
                            if (key[i]) {
                                v[x] = i;
                                pc+=2;
                                break;
                            }
                        }
                        break;
                    case 0x0015:
                        delay = v[x];
                        pc+=2;
                        break;
                    case 0x0018:
                        sound = v[x];
                        pc+=2;
                        break;
                    case 0x001E:
                        I += v[x];
                        pc+=2;
                        break;
                    case 0x0029:
                        I = (v[x]&0x000F) * 5;
                        pc+=2;
                        break;
                    case 0x0033:
                        memory->at(I) = (int) v[x] / 100;
                        memory->at(I+1) = ((int) v[x] % 100)/10;
                        memory->at(I+2) = (int) v[x] % 10;
                        pc+=2;
                        break;
                    case 0x0055:
                        for (int i = 0; i <= (x);i++) {
                            memory->at(I+i) = v[i];
                        }
                        pc+=2;
                        break;
                    case 0x0065:
                        for (int i = 0; i <= (x);i++) {
                            v[i] = memory->at(I+i);
                        }
                        pc+=2;
                        break;
                }
        }
    }
    
    static bool getBit(std::uint8_t byte, std::uint8_t pos) {
        if (pos > 8) {
            return (byte >> 7)&1;
        }
        if (pos < 1) {
            return byte & 1;
        }
        return (byte >> (pos-1)) & 1;
    }
    
    void updateTimers() {
        if (delay > 0) {
            delay--;
        }
        if (sound > 0) {
            sound--;
        }
    }
    
    void readFromFile(std::string str) {
        std::FILE *fileHandle = nullptr;
        fileHandle = std::fopen(str.data(), "r");
        if (fileHandle) {
            std::fread(&(memory->at(512)), 1, 3584, fileHandle);
            std::fclose(fileHandle);
        }

    }
    
    void draw(SDL_Renderer *renderer) { // Each pixel = 10x10 rectangle (window is 640x320)
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect;
        rect.w = 10;
        rect.h = 10;
        for (int i = 0; i < 2048;i++) {
            rect.x = (i%64) * rect.w;
            rect.y = (i/64)*rect.h;
            if (display->at(i))
                SDL_RenderFillRect(renderer, &rect);
        }
        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
    
    void handleInput(SDL_Event &event) { // Map 1234qwerasdfzxv to 123c456d789ea0bf
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        key[0x1] = true;
                        break;
                    case SDLK_2:
                        key[0x2] = true;
                        break;
                    case SDLK_3:
                        key[0x3] = true;
                        break;
                    case SDLK_4:
                        key[0xC] = true;
                        break;
                    case SDLK_q:
                        key[0x4] = true;
                        break;
                    case SDLK_w:
                        key[0x5] = true;
                        break;
                    case SDLK_e:
                        key[0x6] = true;
                        break;
                    case SDLK_r:
                        key[0xD] = true;
                        break;
                    case SDLK_a:
                        key[0x7] = true;
                        break;
                    case SDLK_s:
                        key[0x8] = true;
                        break;
                    case SDLK_d:
                        key[0x9] = true;
                        break;
                    case SDLK_f:
                        key[0xE] = true;
                        break;
                    case SDLK_z:
                        key[0xA] = true;
                        break;
                    case SDLK_x:
                        key[0x0] = true;
                        break;
                    case SDLK_c:
                        key[0xB] = true;
                        break;
                    case SDLK_v:
                        key[0xF] = true;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        key[0x1] = false;
                        break;
                    case SDLK_2:
                        key[0x2] = false;
                        break;
                    case SDLK_3:
                        key[0x3] = false;
                        break;
                    case SDLK_4:
                        key[0xC] = false;
                        break;
                    case SDLK_q:
                        key[0x4] = false;
                        break;
                    case SDLK_w:
                        key[0x5] = false;
                        break;
                    case SDLK_e:
                        key[0x6] = false;
                        break;
                    case SDLK_r:
                        key[0xD] = false;
                        break;
                    case SDLK_a:
                        key[0x7] = false;
                        break;
                    case SDLK_s:
                        key[0x8] = false;
                        break;
                    case SDLK_d:
                        key[0x9] = false;
                        break;
                    case SDLK_f:
                        key[0xE] = false;
                        break;
                    case SDLK_z:
                        key[0xA] = false;
                        break;
                    case SDLK_x:
                        key[0x0] = false;
                        break;
                    case SDLK_c:
                        key[0xB] = false;
                        break;
                    case SDLK_v:
                        key[0xF] = false;
                        break;
                }
                break;
        }
    }
    
private:
    // vm variables
    std::unique_ptr<std::array<std::uint8_t,4096>> memory;
    std::unique_ptr<std::array<std::uint8_t,2048>> display;

    std::array<uint8_t,16> v;
    std::uint16_t I;
    std::uint8_t delay;
    std::uint8_t sound;
    
    std::stack<uint16_t> stack;
    std::uint16_t pc;
    
    std::array<uint8_t,16> key;
    // For easy access
    std::uint16_t opcode;
    
    std::uint8_t x;
    std::uint8_t y;
    std::uint16_t nnn;
    std::uint8_t n;
    std::uint8_t kk;
};

int main(int argc, const char * argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    emulator chip8;
    SDL_Event event;
    bool run=true;
    
    double oldTime1=0;
    double newTime1=0;
    double oldTime2 = 0;
    double newTime2 = 0;
    
    if (argc >= 2) {
        chip8.readFromFile(argv[1]);
    }
    while(run) {
        newTime1 = SDL_GetTicks();
        newTime2 = SDL_GetTicks();
        if (newTime1 >= oldTime1+(1/60.0)) { // Draws per second
            oldTime1 = newTime1;
            chip8.updateTimers();
            chip8.draw(renderer);
        }

        if (newTime2 >= oldTime2+(1/100.0)) { // Instructions per second
            oldTime2 = newTime2;
            chip8.fetch();
            chip8.execute();
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run=false;
            }
            chip8.handleInput(event);
        }
    }
    return 0;
}


