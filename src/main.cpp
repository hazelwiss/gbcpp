/*
    Gameboy colour emulator written in C++
*/
#include<interpreter.h>
#include<core/instructions.h>
#include<display/display.h>

int main(){
    main_window::init();
    interpreter_t interp;
    interp.load_rom("roms/test/gb-test-roms/cpu_instrs/individual/09-op r,r.gb");
    main_window::bind(interp);
    main_window::start();
    while(true){
        interp.update();
    }
}