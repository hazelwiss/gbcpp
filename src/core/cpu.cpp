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
    instr_info.opcode = mem.read(pc);
    if(instr_info.opcode==0xCB){
        instr = instr_table::cb_range[instr_info.ext_opcode = mem.read(pc+1)];
        instr_size = 2;
    } else{
        instr = instr_table::noncb_range[instr_info.opcode];
        instr_size = entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
        switch(instr_size){
        case 2: instr_info.imm8     = mem.read(pc+1);                     break;
        case 3: instr_info.imm16    = mem.read(pc+1)|(mem.read(pc+2)<<8); break;
        }
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
    timer.tick_t_cycles(arg.did_branch ? cycles.first : cycles.second);
    //  debugging
    if(instruction_execute_callbk){
        instruction_execute_callbk(prev_pc, 
            disasm.disassemble(instr_info.opcode, prev_pc+instr_size, instr_info.imm16));
    }
    if(disassembler_t::is_call(instr_info.opcode) && arg.did_branch){
        if(enter_call_callbk)
            enter_call_callbk(prev_pc, pc);
    } else if(disassembler_t::is_ret(instr_info.opcode) && arg.did_branch){
        if(ret_from_call_callbk)
            ret_from_call_callbk();
    }
    if(code_breakpoints.size() > 0){
        if(code_breakpoints.contains(pc)){
            if(code_breakpoints[pc] && code_breakpoints_callbk)
                code_breakpoints_callbk(pc, instr_info.opcode, instr_info.imm16);
        }
    }

}