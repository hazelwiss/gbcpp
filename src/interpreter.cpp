#include<interpreter.h>
#include<core/instructions.h>
#include<stdexcept>
#include<iostream>
#include<disassemble/disassemble.h>
#include<iostream>

void interpreter_t::update(){
    //  test loop
    while(true){
        auto& pc = cpu.regs.get<RI::PC>();
        uint8_t opc = mem.read(pc);
        cpu_function_entry instr;
        disassembler_t disasm;
        cpu_function_argument_t arg{cpu,mem};
        std::cout << pc << ": ";
        if(opc==0xCB){
            uint8_t cb_opc = mem.read(pc+1);
            instr = instr_table::cb_range[cb_opc];
            std::cout << disasm.disassemble_cb(cb_opc) << std::endl;
        }
        else{
            instr = instr_table::noncb_range[opc];
            cpu.instr_info.imm16 = mem.read(pc+1)|(mem.read(pc+2)<<8);
            std::cout << disasm.disassemble(opc,mem.read(pc+1)|(mem.read(pc+2)<<8)) << std::endl;
        }
        pc+=entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
        try{
            entry_get<CPU_ENTRY::FUNCTION>(instr)(arg);
        } catch(std::runtime_error& e){
            std::cout << "application quit with the following exception:" << std::endl;
            std::cout << e.what() << std::endl;
            std::abort();
        }
        auto ticks = entry_get<CPU_ENTRY::TICKS>(instr);
        cpu.timer.tick_t_cycles(arg.did_branch ? ticks.first : ticks.second);
        mem.write(0xFF44, 0x90);    //  tmp
        if(pc == 0xD000)
            int x = 0;
    }
}