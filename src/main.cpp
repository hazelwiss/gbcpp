/*
    Gameboy colour emulator written in C++
*/
#include<gameboy.h>
#include<core/instructions.h>
#include<display/display.h>

int main(){
    gameboy_t gb;
    gb.load_rom("roms/test/gb-test-roms/cpu_instrs/cpu_instrs.gb");
    main_window::bind(gb);
    main_window::init();
    main_window::start();
    while(true){
        gb.update();
    }
}