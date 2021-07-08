#include<core/cpu.h>
#include<core/instructions.h>
#include<stdexcept>
#include<iostream>
#include<iostream>

void cpu_t::tick_step(size_t ticks){
    auto& pc = regs.get<RI::PC>();
    auto prev_pc = pc;
    uint8_t opc = mem.read(pc);
    cpu_function_entry instr;
    cpu_function_argument_t arg{*this,*mem};
    instr_info.imm16 = mem.read(pc+1)|(mem.read(pc+2)<<8);
    if(opc==0xCB){
        uint8_t cb_opc = mem.read(pc+1);
        instr = instr_table::cb_range[cb_opc];
    } else
        instr = instr_table::noncb_range[opc];
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
    //  debugging
    if(instruction_execute_callbk){
        instruction_execute_callbk(prev_pc, disasm.disassemble(opc, 
            prev_pc+entry_get<CPU_ENTRY::BYTE_LENGTH>(instr), instr_info.imm16));
    }
    if(disassembler_t::is_call(opc) && arg.did_branch){
        if(enter_call_callbk)
            enter_call_callbk(prev_pc, pc);
    } else if(disassembler_t::is_ret(opc) && arg.did_branch){
        if(ret_from_call_callbk)
            ret_from_call_callbk();
    }
    if(code_breakpoints.size() > 0){
        if(code_breakpoints.contains(pc)){
            if(code_breakpoints[pc] && code_breakpoints_callbk)
                code_breakpoints_callbk(pc, opc, instr_info.imm16);
        }
    }

}