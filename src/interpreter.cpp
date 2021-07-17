#include<interpreter.h>
#include<display/display.h>
#include<chrono>

interpreter_t::interpreter_t(){
    init();
}

void interpreter_t::init(){
    //  memory callbacks.
    mem.unbind_bootrom_callbk = [&](){
        main_window::bind(*this);
    };
    mem.read_breakpoint_callbk = [&](uint16_t adr){
        paused = true;
        main_window::on_pause();
    };
    mem.write_breakpoint_callbk = [&](uint16_t adr, uint8_t val){
        paused = true;
        main_window::on_pause();
    };
    //  cpu callbacks.
    cpu.code_breakpoints_callbk = [&](uint16_t adr, uint8_t opc, uint16_t imm){
        paused = true;
        main_window::on_pause();
    };
    cpu.enter_call_callbk = [&](uint16_t p_pc, uint16_t c_pc){
        call_deque.push_front({p_pc, c_pc});
    };
    cpu.ret_from_call_callbk = [&](){
        if(call_deque.size())
            call_deque.pop_front();
    };
    cpu.instruction_execute_callbk = [&](uint16_t adr, const std::string& mnemonic){
        recent_instr_deque.push_front({adr, mnemonic});
        if(recent_instr_deque.size() > 14)
            recent_instr_deque.pop_back();
    };
    //  tmp
    cpu.code_breakpoints[0x101] = true;
}

void interpreter_t::reset(){
    mutex.lock();
    std::string cur_rom = this->mem.get_rom_path();
    this->mem = memory_t{};
    this->load_rom(cur_rom);
    auto breakpoints = this->cpu.code_breakpoints;
    this->cpu = cpu_t{this->mem};
    this->cpu.code_breakpoints = breakpoints;
    this->scheduler = scheduler_t{};
    this->call_deque.clear();
    this->recent_instr_deque.clear();
    this->paused = false;
    init();
    mutex.unlock();
}

void interpreter_t::update(){
    while(paused){
        if(should_step){
            should_step = false;
            break;
        }
    }
    mutex.lock();
    cpu.tick_step(0);
    mutex.unlock();
}