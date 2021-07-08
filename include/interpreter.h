/*
    container class for the interpreter. Keeps the scheduler to effectively tick every component.
*/
#pragma once
#include<common_defs.h>
#include<scheduler.h>
#include<core/cpu.h>
#include<memory/memory.h>
#include<atomic>
#include<deque>
#include<mutex>

struct interpreter_t{
    interpreter_t();
    void update();
    void load_rom(const std::string& path){ mem.load_rom(path); }
protected:
    void on_unpause(){ if(lock.owns_lock()) lock.unlock(); }
    void on_pause(){ if(!lock.owns_lock()) lock.lock(); }
    memory_t mem;
    cpu_t cpu{mem};
    scheduler_t scheduler;
    std::atomic_bool paused{false};
    std::atomic_bool should_step{false};
    std::mutex mutex;
    std::unique_lock<std::mutex> lock{mutex};   //  gives exclusive access to memory for this thread.
    std::atomic_uint64_t fps;
    std::deque<std::pair<uint16_t,uint16_t>> call_deque;
    std::deque<std::pair<uint16_t,std::string>> recent_instr_deque;
    friend struct dbg_window;
};
