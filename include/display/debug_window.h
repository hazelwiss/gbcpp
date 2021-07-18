#pragma once
#include<common_defs.h>
#include<gameboy.h>
#include<string>
#include<functional>

struct dbg_window{
    static const std::string imgui_win_id;
    static void hook(gameboy_t& gb);
    static void draw();
    static void reset_disasm();
    static float size_x,size_y;
    static void on_pause();
    static void on_play();
protected:
    static void draw_reg_subwindow();
    static void draw_disasm_subwindow();
    static void draw_control_subwindow();
    static void disassemble(uint16_t adr=0);
    static gameboy_t* gameboy;
    static inline uint16_t upper_viewable_rom_address{0x7FFF};
};
