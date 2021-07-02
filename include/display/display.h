#pragma once

struct interpreter_t;

struct main_window{
    static void init(interpreter_t& interp);
    static void draw();
    static bool enable_debug_window;
protected:
    static void draw_main_menu();
};