#include<core/cpu.h>
#include<core/instructions.h>
#include<stdexcept>
#include<iostream>
#include<iostream>


void cpu_t::tick_step(size_t ticks){
    auto& pc = regs.get<RI::PC>();
    auto prev_pc = pc;
    cpu_function_argument_t arg{*this,*mem};
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
    timer.tick_t_cycles(arg.did_branch ? cycles.second : cycles.first);
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