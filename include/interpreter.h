/*
    container class for the interpreter. Keeps the scheduler to effectively tick every component.
*/
#pragma once
#include<common_defs.h>
#include<scheduler.h>
#include<core/cpu.h>
#include<memory/memory.h>
#include<atomic>
#include<stack>

struct interpreter_t{
    interpreter_t();
    void update();
    void load_rom(const std::string& path){ mem.load_rom(path); }
protected:
    memory_t mem;
    cpu_t cpu{mem};
    scheduler_t scheduler;
    __DEBUG_LINE(
        std::atomic_bool paused{false};
        std::atomic_bool should_step{false};
        std::atomic_uint64_t fps;
        std::stack<std::pair<uint16_t,uint16_t>> call_stack;
        friend struct dbg_window;
    )
};
