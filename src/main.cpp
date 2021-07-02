/*
    Gameboy colour emulator written in C++
*/
#include<interpreter.h>
#include<core/instructions.h>

int main(){
    interpreter_t interp;
    interp.load_rom("roms/test/gb-test-roms/cpu_instrs/individual/09-op r,r.gb");
    interp.update();
}