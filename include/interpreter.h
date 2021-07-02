/*
    container class for the interpreter. Keeps the scheduler to effectively tick every component.
*/
#pragma once
#include<common_defs.h>
#include<scheduler.h>
#include<core/cpu.h>
#include<memory/memory.h>

struct interpreter_t{
    void update();
    void load_rom(const std::string& path){ mem.load_rom(path); }
protected:
    cpu_t cpu;
    memory_t mem;
    scheduler_t scheduler;
    friend struct dbg_window;
};
