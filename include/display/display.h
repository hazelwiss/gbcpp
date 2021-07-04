#pragma once
#include<common_defs.h>
#include<atomic>

struct interpreter_t;

struct main_window{
    static void init();
    static void bind(interpreter_t& interp);
    static void start();
    static void stop();
    static void draw();
    static void on_pause();
    static std::atomic_bool enable_debug_window;
    static interpreter_t placeholder;
    template<typename t, typename... _args>
    static void handshake(const t& f, bool skip){
        if(!enable_debug_window || skip){
            f();
        } else{
            request_handshake = true;
            while(!allow_handshake);
            f();
            request_handshake = false;
            while(allow_handshake);
        }

    };
protected:
    static std::atomic_bool request_handshake;
    static std::atomic_bool allow_handshake;
    static void threaded_loop();
    static void draw_main_menu();
    static std::atomic_bool enable_display;
};
