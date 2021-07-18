#include<scheduler.h>

bool scheduler_t::is_event_pending(){
    //return cycles >= events.top().first;
    return false;
}

void scheduler_t::process_events(){
    //events.top().second();
    //events.pop();
}

void scheduler_t::add_event(std::pair<timestamp_t, event_t> e){
    //events.push(e);
}

void scheduler_t::tick_system(size_t t_cycles){
    //cycles += t_cycles;
}