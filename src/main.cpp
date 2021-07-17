/*
    Gameboy colour emulator written in C++
*/
#include<interpreter.h>
#include<core/instructions.h>
#include<display/display.h>

int main(){
    interpreter_t interp;
    interp.load_rom("roms/test/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb");
    main_window::bind(interp);
    main_window::init();
    main_window::start();
    while(true){
        interp.update();
    }
}