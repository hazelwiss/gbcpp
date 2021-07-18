#include<gameboy.h>
#include<display/display.h>
#include<core/instructions.h>
#include<stdexcept>
#include<chrono>
#include<iostream>

gameboy_t::gameboy_t(){
    init();
}

void gameboy_t::init(){
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
    code_breakpoints_callbk = [&](uint16_t adr, uint8_t opc, uint16_t imm){
        paused = true;
        main_window::on_pause();
    };
    enter_call_callbk = [&](uint16_t p_pc, uint16_t c_pc){
        call_deque.push_front({p_pc, c_pc});
    };
    ret_from_call_callbk = [&](){
        if(call_deque.size())
            call_deque.pop_front();
    };
    instruction_execute_callbk = [&](uint16_t adr, const std::string& mnemonic){
        recent_instr_deque.push_front({adr, mnemonic});
        if(recent_instr_deque.size() > 14)
            recent_instr_deque.pop_back();
    };
    //  tmp
    code_breakpoints[0x101] = true;
}

void gameboy_t::reset(){
    mutex.lock();
    std::string cur_rom = this->mem.get_rom_path();
    this->mem = memory_t{};
    this->load_rom(cur_rom);
    auto breakpoints = this->code_breakpoints;
    this->regs = cpu_register_bank_t{};
    this->scheduler = scheduler_t{};
    this->ime = false;
    this->ime_change = false;
    this->code_breakpoints = breakpoints;
    this->scheduler = scheduler_t{};
    this->call_deque.clear();
    this->recent_instr_deque.clear();
    this->paused = false;
    init();
    mutex.unlock();
}

void gameboy_t::update(){
    DEBUG_CALL(
        while(paused){
            if(should_step){
                should_step = false;
                break;
            }
        }
    );
    mutex.lock();
    while(scheduler.is_event_pending()){
        scheduler.process_events();
    }
    fetch_decode_execute();
    mutex.unlock();
}

void gameboy_t::fetch_decode_execute(){
    auto& pc = regs.get<RI::PC>();
    auto prev_pc = pc;
    cpu_function_argument_t arg{*this};
    cpu_function_entry instr;
    size_t instr_size;
    uint8_t opcode = mem.read(pc);
    if(opcode==0xCB){
        instr = instr_table::cb_range[mem.read(pc+1)];
        instr_size = 2;
    } else{
        instr = instr_table::noncb_range[opcode];
        instr_size = entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
    }
    pc += instr_size;
    try{
        entry_get<CPU_ENTRY::FUNCTION>(instr)(arg);
    } catch(std::runtime_error& e){
        std::cout << "application quit with the following exception:" << std::endl;
        std::cout << e.what() << std::endl;
        std::abort();
    }
    auto cycles = entry_get<CPU_ENTRY::CYCLES>(instr);
    //scheduler.tick_system(arg.did_branch ? cycles.first : cycles.second);
    //  debugging
    DEBUG_CALL(
        if(instruction_execute_callbk){
            instruction_execute_callbk(prev_pc, 
                disasm.disassemble(opcode, prev_pc+instr_size, immediate16()));
        }
        if(disassembler_t::is_call(opcode) && arg.did_branch){
            if(enter_call_callbk)
                enter_call_callbk(prev_pc, pc);
        } else if(disassembler_t::is_ret(opcode) && arg.did_branch){
            if(ret_from_call_callbk)
                ret_from_call_callbk();
        }
        if(code_breakpoints.size() > 0){
            if(code_breakpoints.contains(pc)){
                if(code_breakpoints[pc] && code_breakpoints_callbk)
                    code_breakpoints_callbk(pc, opcode, immediate16());
            }
        }
    );
}

uint8_t gameboy_t::immediate8(){
    return mem.read(regs.get<RI::PC>()-1); 
}

uint16_t gameboy_t::immediate16(){
    return mem.read(regs.get<RI::PC>() - 2) | (mem.read(regs.get<RI::PC>() - 1) <<8);
}