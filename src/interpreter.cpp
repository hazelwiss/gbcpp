#include<interpreter.h>
#include<display/display.h>

interpreter_t::interpreter_t(){
    mem.bind_unbind_bootrom_callbk([&](){
        main_window::bind(*this);
    });
}

void interpreter_t::update(){

}