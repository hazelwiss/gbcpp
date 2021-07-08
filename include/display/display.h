#pragma once
#include<common_defs.h>
#include<atomic>
#include<condition_variable>

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
    static void handshake(std::mutex& m){
        cv.notify_one();
    };
protected:
    static void threaded_loop();
    static void draw_main_menu();
    static std::atomic_bool enable_display;
    static std::condition_variable cv; 
    static interpreter_t* interp_ref;
};
