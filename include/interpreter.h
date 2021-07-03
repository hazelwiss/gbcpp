/*
    container class for the interpreter. Keeps the scheduler to effectively tick every component.
*/
#pragma once
#include<common_defs.h>
#include<scheduler.h>
#include<core/cpu.h>
#include<memory/memory.h>

struct interpreter_t{
    interpreter_t();
    void update();
    void load_rom(const std::string& path){ mem.load_rom(path); }
protected:
    memory_t mem;
    cpu_t cpu{mem};
    scheduler_t scheduler;
    friend struct dbg_window;
};
