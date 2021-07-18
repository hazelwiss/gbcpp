#pragma once
#include<common_defs.h>
#include<gameboy.h>
#include<memory/memory.h>
#include<array>
#include<tuple>

struct cpu_function_argument_t{
    cpu_function_argument_t(gameboy_t& gb): gb{gb} {}
    gameboy_t& gb;
    bool did_branch{false};
};
using cfa = cpu_function_argument_t&;

using _cpu_function_prototype = void(*)(cfa);
using _instr_br_ticks    = size_t;
using _instr_ticks       = size_t;
using _instr_ticks_entry = std::pair<_instr_ticks, _instr_br_ticks>;
using cpu_function_entry = std::tuple<size_t,  //  1: byte length of instruction 
    _instr_ticks_entry,                         //  2: pair of (1) cycle with no branch (2) cycles with branch
    _cpu_function_prototype>;                   //  3: funcion pointer;    

enum class CPU_ENTRY{ BYTE_LENGTH,CYCLES,FUNCTION };
template<CPU_ENTRY entry>
constexpr auto& entry_get(cpu_function_entry e){
    return std::get<(uint32_t)entry>(e);
} 

constexpr size_t INSTR_ARRAY_SIZE = 256;
using _instr_array = std::array<
    cpu_function_entry,
    INSTR_ARRAY_SIZE>;

namespace instr_table{
    extern const _instr_array noncb_range;
    extern const _instr_array cb_range; 
}