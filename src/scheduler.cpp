#include<scheduler.h>
#include<stdexcept>

bool scheduler_t::is_event_pending(){
    if(!events.size())
        return false;
    return cycles >= events.top().first;
}

void scheduler_t::process_events(){
    if(!events.size())
        throw std::runtime_error("sheculder event queue is empty when trying to process events.");
    events.top().second.f();
    events.pop();
}

void scheduler_t::add_event(const event_pair_t& e){
    events.push({e.first+cycles, e.second});
}

void scheduler_t::tick_system(size_t t_cycles){
    cycles += t_cycles;
}

size_t scheduler_t::get_cycles(){
    return cycles;
}