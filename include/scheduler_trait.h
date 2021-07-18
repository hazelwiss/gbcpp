#pragma once
#include<concepts>
#include<scheduler.h>

template<typename derived>
struct scheduler_component_trait{
    scheduler_component_trait() { post(); }
private:
    void post(){
        static_assert(requires(derived s){ s.update(); }, 
            "a scheduler component must implement void update() non-static member function.");
    }
};
