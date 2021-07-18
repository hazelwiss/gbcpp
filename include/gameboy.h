/*
    container class for the interpreter. Keeps the scheduler to effectively tick every component.
*/
#pragma once
#include<common_defs.h>
#include<scheduler.h>
#include<memory/memory.h>
#include<core/interpreter.h>
#include<atomic>
#include<mutex>

struct gameboy_t{
    gameboy_t();
    void update();
    void load_rom(const std::string& path){ mem.load_rom(path); }
    memory_t mem;
    cpu_register_bank_t regs;
    uint8_t immediate8();
    uint16_t immediate16();
    scheduler_t scheduler;
    bool ime{false}, ime_change{false}, ime_change_to_value;
    bool halted{false},stopped{false};
protected:
    void init();
    void reset();
    void on_unpause(){}
    void on_pause(){}
    void fetch_decode_execute();
    std::atomic_bool paused{false};
    std::atomic_bool should_step{false};
    std::mutex mutex;
    std::atomic_uint64_t fps{0};
    //  debugging.
    disassembler_t disasm;
    std::unordered_map<uint16_t, bool> code_breakpoints;
    std::deque<std::pair<uint16_t,uint16_t>> call_deque;
    std::deque<std::pair<uint16_t,std::string>> recent_instr_deque;
    std::function<void(uint16_t, uint8_t, uint16_t)> code_breakpoints_callbk;
    std::function<void(uint16_t, const std::string&)> instruction_execute_callbk;
    std::function<void(uint16_t, uint16_t)> enter_call_callbk;
    std::function<void()> ret_from_call_callbk;
    friend struct dbg_window;
};
