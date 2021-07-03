#include<core/cpu.h>
#include<core/instructions.h>
#include<stdexcept>
#include<iostream>
#include<disassemble/disassemble.h>
#include<iostream>

void cpu_t::tick_step(size_t ticks){
    auto& pc = regs.get<RI::PC>();
    uint8_t opc = mem.read(pc);
    cpu_function_entry instr;
    cpu_function_argument_t arg{*this,mem};
    if(opc==0xCB){
        uint8_t cb_opc = mem.read(pc+1);
        instr = instr_table::cb_range[cb_opc];
    } else{
        instr = instr_table::noncb_range[opc];
        instr_info.imm16 = mem.read(pc+1)|(mem.read(pc+2)<<8);
    }
    pc+=entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
    try{
        entry_get<CPU_ENTRY::FUNCTION>(instr)(arg);
    } catch(std::runtime_error& e){
        std::cout << "application quit with the following exception:" << std::endl;
        std::cout << e.what() << std::endl;
        std::abort();
    }
    auto cycles = entry_get<CPU_ENTRY::CYCLES>(instr);
    timer.tick_t_cycles(arg.did_branch ? cycles.first : cycles.second);
    mem.write(0xFF44, 0x90);    //  tmp solution to pass boot rom.
}