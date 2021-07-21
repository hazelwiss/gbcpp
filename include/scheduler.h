/*
    scheduler for effectively ticking every component
*/
#pragma once
//  custom header
#include<common_defs.h>
#include<queue>
#include<utility>
#include<functional>

enum class scheduler_event{
    IF_WRITE,
    IE_WRITE,
    EI,
    DI  
};

using timestamp_t = size_t;
struct event_t{ std::function<void()> f; scheduler_event event; };

struct event_pair_t: public std::pair<timestamp_t, event_t>{
    using std::pair<timestamp_t, event_t>::pair;
    bool operator<(const event_pair_t& arg) const{ return first < arg.first; }
};

struct scheduler_t{
    bool is_event_pending();
    void process_events();
    void add_event(const event_pair_t&);
    void tick_system(size_t t_cycles);
    size_t get_cycles();
protected:
    uint64_t cycles;
    std::priority_queue<event_pair_t> events;
};

