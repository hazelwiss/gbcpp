/*
    Gameboy colour emulator written in C++
*/
#include<interpreter.h>
#include<core/instructions.h>
#include<display/display.h>

int main(){
    interpreter_t interp;
    main_window::init(interp);
    interp.load_rom("roms/test/gb-test-roms/cpu_instrs/individual/09-op r,r.gb");
    while(true){
        main_window::draw();
        interp.update();
    }
}