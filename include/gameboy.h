/*
    container class for the interpreter. Keeps the scheduler to effectively tick every component.
*/
#pragma once
#include<common_defs.h>
#include<scheduler.h>
#include<memory/memory.h>
#include<core/interpreter.h>
#include<deque>

struct gameboy_t{
    gameboy_t();
    void update();
    void load_rom(const std::string& path){ mem.load_rom(path); }
    void handle_interrupts();
    uint8_t immediate8();
    uint16_t immediate16();
    scheduler_t scheduler;
    memory_t mem{this};
    cpu_register_bank_t regs;
    bool ime{false};
    bool halted{false},stopped{false};
protected:
    void init();
    void fetch_decode_execute();
    size_t fps{0};
    //  debugging.
    void dbg_reset();
    bool dbg_paused{false};
    bool dbg_should_step{false};
    disassembler_t dbg_disasm;
    std::unordered_map<uint16_t, bool> dbg_code_breakpoints;
    std::deque<std::pair<uint16_t,uint16_t>> dbg_call_deque;
    std::deque<std::pair<uint16_t,std::string>> dbg_recent_instr_deque;
    std::function<void(uint16_t, uint8_t, uint16_t)> dbg_code_breakpoints_callbk;
    std::function<void(uint16_t, const std::string&)> dbg_instruction_execute_callbk;
    std::function<void(uint16_t, uint16_t)> dbg_enter_call_callbk;
    std::function<void()> dbg_ret_from_call_callbk;
    friend struct dbg_window;
};
