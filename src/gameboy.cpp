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
#ifdef __DEBUG__
    //  memory callbacks.
    mem.dbg_unbind_bootrom_callbk = [&](){
        main_window::bind(*this);
    };
    mem.dbg_read_breakpoint_callbk = [&](uint16_t adr){
        dbg_paused = true;
        main_window::on_pause();
    };
    mem.dbg_write_breakpoint_callbk = [&](uint16_t adr, uint8_t val){
        dbg_paused = true;
        main_window::on_pause();
    };
    //  cpu callbacks.
    dbg_code_breakpoints_callbk = [&](uint16_t adr, uint8_t opc, uint16_t imm){
        dbg_paused = true;
        main_window::on_pause();
    };
    dbg_enter_call_callbk = [&](uint16_t p_pc, uint16_t c_pc){
        dbg_call_deque.push_front({p_pc, c_pc});
    };
    dbg_ret_from_call_callbk = [&](){
        if(dbg_call_deque.size())
            dbg_call_deque.pop_front();
    };
    dbg_instruction_execute_callbk = [&](uint16_t adr, const std::string& mnemonic){
        dbg_recent_instr_deque.push_front({adr, mnemonic});
        if(dbg_recent_instr_deque.size() > 14)
            dbg_recent_instr_deque.pop_back();
    };
    dbg_code_breakpoints[0x101] = true;
#endif
}

void gameboy_t::dbg_reset(){
    std::mutex mut{};   //  needs to use as gameboy's mutex will be destructed.
    mut.lock();
    this->dbg_mutex.unlock();
    std::string cur_rom = this->mem.get_rom_path();
    auto breakpoints = this->dbg_code_breakpoints;
    *this = gameboy_t{};
    this->dbg_code_breakpoints = breakpoints;
    this->load_rom(cur_rom);
    mut.unlock();
}

void gameboy_t::update(){
#ifdef __DEBUG__
    while(dbg_paused){
        if(dbg_should_step){
            dbg_should_step = false;
            break;
        }
    }
#endif
    dbg_mutex.lock();
    while(scheduler.is_event_pending()){
        scheduler.process_events();
    }
    fetch_decode_execute();
    dbg_mutex.unlock();
}

void gameboy_t::fetch_decode_execute(){
    auto& pc = regs.get<RI::PC>();
#ifdef __DEBUG__
    auto prev_pc = pc;
#endif
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
    scheduler.tick_system(arg.did_branch ? cycles.first : cycles.second);
    //  debugging
#ifdef __DEBUG__
    if(dbg_instruction_execute_callbk){
        dbg_instruction_execute_callbk(prev_pc, 
            dbg_disasm.disassemble(opcode, prev_pc+instr_size, immediate16()));
    }
    if(disassembler_t::is_call(opcode) && arg.did_branch){
        if(dbg_enter_call_callbk)
            dbg_enter_call_callbk(prev_pc, pc);
    } else if(disassembler_t::is_ret(opcode) && arg.did_branch){
        if(dbg_ret_from_call_callbk)
            dbg_ret_from_call_callbk();
    }
    if(dbg_code_breakpoints.size() > 0){
        if(dbg_code_breakpoints.contains(pc)){
            if(dbg_code_breakpoints[pc] && dbg_code_breakpoints_callbk)
                dbg_code_breakpoints_callbk(pc, opcode, immediate16());
        }
    }
#endif
}

uint8_t gameboy_t::immediate8(){
    return mem.read(regs.get<RI::PC>()-1); 
}

uint16_t gameboy_t::immediate16(){
    return mem.read(regs.get<RI::PC>() - 2) | (mem.read(regs.get<RI::PC>() - 1) <<8);
}

void gameboy_t::handle_interrupts(){
    auto ie_var = mem.read(IE_ADR); 
    auto if_var = mem.read(IF_ADR);
    if(ie_var & if_var & 0x1F){
        if(halted){
            halted = false;
            regs.get<RI::PC>()+=1;
            scheduler.tick_system(4);
        }
        if(ime){
            scheduler.tick_system(20);  //  tick 5 machine cycles
            auto& sp = regs.get<RI::SP>();
            mem.write(--sp, regs.get<RI::PC>() >> 8);
            mem.write(--sp, regs.get<RI::PC>());
            switch(if_var & ie_var){
            case 0x01: 
                regs.get<RI::PC>() = 0x40; 
                mem.write(IF_ADR, if_var ^ 0x01);
                break;
            case 0x02:
                regs.get<RI::PC>() = 0x48;
                mem.write(IF_ADR, if_var ^ 0x02);
                break;
            case 0x04:
                regs.get<RI::PC>() = 0x50;
                mem.write(IF_ADR, if_var ^ 0x04);
                break;
            case 0x08:
                regs.get<RI::PC>() = 0x58;
                mem.write(IF_ADR, if_var ^ 0x08);
                break;
            case 0x10:
                regs.get<RI::PC>() = 0x60;
                mem.write(IF_ADR, if_var ^ 0x10);
            }
            ime = false;
        }
    }
}