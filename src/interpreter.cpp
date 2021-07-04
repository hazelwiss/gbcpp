#include<interpreter.h>
#include<display/display.h>
#include<chrono>

interpreter_t::interpreter_t(){
    __DEBUG_LINE(
        //  memory callbacks.
        mem.unbind_bootrom_callbk = [&](){
            main_window::bind(*this);
        };
        mem.read_breakpoint_callbk = [&](uint16_t adr){
            paused = true;
        };
        mem.write_breakpoint_callbk = [&](uint16_t adr, uint8_t val){
            paused = true;
        };
        //  cpu callbacks.
        cpu.code_breakpoints_callbk = [&](uint16_t adr, uint8_t opc, uint16_t imm){
            paused = true;
        };
        cpu.enter_call_callbk = [&](uint16_t p_pc, uint16_t c_pc){
            call_stack.push({p_pc, c_pc});
        };
        cpu.ret_from_call_callbk = [&](){
            call_stack.pop();
        };
        //  tmp
        cpu.code_breakpoints[0x101] = true;
    )
}

void interpreter_t::update(){
    using namespace std::chrono;
    __DEBUG_LINE(auto time = high_resolution_clock::now());
    if(!paused||should_step){
        cpu.tick_step(0);
    } 
    if(should_step){
        should_step = false;
    }
    __DEBUG_LINE(fps = 1.0_n/std::chrono::duration_cast<std::chrono::nanoseconds>(high_resolution_clock::now()-time).count());
}