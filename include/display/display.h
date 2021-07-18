#pragma once
#include<common_defs.h>
#include<atomic>

struct gameboy_t;

struct main_window{
    static void init();
    static void bind(gameboy_t& interp);
    static void start();
    static void stop();
    static void draw();
    static void on_pause();
    static std::atomic_bool enable_debug_window;
protected:
    static void threaded_loop();
    static void draw_main_menu();
    static std::atomic_bool enable_display;
};
