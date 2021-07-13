/*
    contains the processor loop amongst other things
*/
#pragma once
#include<common_defs.h>
#include<memory/memory.h>
#include<scheduler_trait.h>
#include<disassemble/disassemble.h>

union cpu_register_t{
    struct{
        uint8_t _l;
        uint8_t _h;
    } _narrow; 
    uint16_t _wide;
};

struct flags_t{
    uint8_t _unused:4{0};
    uint8_t c:1,h:1,n:1,z:1;
};

union cpu_register_a_t{
    struct{
        flags_t _l;
        uint8_t _h;
    } _narrow; 
    uint16_t _wide;
};

enum class FLAG_INDEX{ Z,N,H,C };
namespace RI{
    enum R8     { A,B,C,D,E,H,L };
    enum R16    { AF,BC,DE,HL,SP,PC };
}
using RI8   = RI::R8;
using RI16  = RI::R16;
using FI = FLAG_INDEX;

struct cpu_register_bank_t{
    template<RI8 r> constexpr uint8_t& get(){
        switch(r){
        case RI::A:     return af._narrow._h;
        case RI::B:     return bc._narrow._h;
        case RI::C:     return bc._narrow._l;
        case RI::D:     return de._narrow._h;
        case RI::E:     return de._narrow._l;
        case RI::H:     return hl._narrow._h;
        case RI::L:     return hl._narrow._l;
        }
    }
    template<RI16 r> constexpr uint16_t& get(){
        switch(r){
        case RI::AF:    return af._wide;
        case RI::BC:    return bc._wide;
        case RI::DE:    return de._wide;
        case RI::HL:    return hl._wide;
        case RI::SP:    return sp._wide;
        case RI::PC:    return pc._wide;
        }
    }
    template<FI f>
    constexpr bool get_flag(){
        switch(f){
        case FI::Z: return af._narrow._l.z != 0;
        case FI::N: return af._narrow._l.n != 0;
        case FI::H: return af._narrow._l.h != 0;
        case FI::C: return af._narrow._l.c != 0;
        }
    }
    template<FI f>
    constexpr void set_flag(bool set){
        switch(f){
        case FI::Z: af._narrow._l.z = (set != 0); break;
        case FI::N: af._narrow._l.n = (set != 0); break;
        case FI::H: af._narrow._l.h = (set != 0); break;
        case FI::C: af._narrow._l.c = (set != 0); break;
        }
    }
private:
    cpu_register_t bc{0},de{0},hl{0},sp{0},pc{0};
    cpu_register_a_t af{0};
};

struct cpu_t: schedule_component_base_t{
    cpu_t(memory_t& mem): mem{&mem} {}
    void tick_step(size_t ticks);
    bool halted{false},stopped{false};
    struct{ 
        void tick_t_cycles(size_t s){ t_cycles+=s; }
        size_t get_t_cycles(){ return t_cycles; }
        size_t get_m_cycles(){ return t_cycles/4; }
    protected:
        size_t t_cycles;
    } timer;
    cpu_register_bank_t regs;
    bool ime{false};
    struct{
        uint8_t opcode;
        uint16_t ext_opcode;    //  used with cb
        union{
            uint16_t imm16;
            uint8_t imm8;
        };
    } instr_info;
    struct memory_wrapper_t{
        memory_wrapper_t(memory_t* mem): mem{mem} {}        
        uint8_t read(uint16_t adr){ return mem->read(adr); }
        void write(uint16_t adr, uint8_t val){ mem->write(adr, val); }
        memory_t& get(){ return *mem; }
        memory_t& operator*(){ return get(); }
        memory_t& operator->(){ return get(); }
    protected:
        memory_t* mem;
    } mem;
    //  debugging.
    disassembler_t disasm;
    std::unordered_map<uint16_t, bool> code_breakpoints;
    std::function<void(uint16_t, uint8_t, uint16_t)> code_breakpoints_callbk;
    std::function<void(uint16_t, const std::string&)> instruction_execute_callbk;
    std::function<void(uint16_t, uint16_t)> enter_call_callbk;
    std::function<void()> ret_from_call_callbk;
};