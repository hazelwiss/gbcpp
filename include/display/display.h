#pragma once
#include<common_defs.h>
#ifdef DEBUG_MODE

struct interpreter_t;

struct main_window{
    static void init();
    static void bind(interpreter_t& interp);
    static void start();
    static void stop();
    static void draw();
    static bool enable_debug_window;
protected:
    static void threaded_loop();
    static void draw_main_menu();
    static bool enable_display;
};

#endif