/*
    scheduler for effectively ticking every component
*/
#pragma once
//  custom header
#include<common_defs.h>
#include<queue>
#include<utility>

using timestamp_t = size_t;
using event_t = void(*)();

struct scheduler_t{
    bool is_event_pending();
    void process_events();
    void add_event(std::pair<timestamp_t, event_t>);
    void tick_system(size_t t_cycles);
protected:
    uint64_t cycles;
    std::priority_queue<std::pair<timestamp_t, event_t>> events;
};

