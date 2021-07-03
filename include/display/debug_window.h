#pragma once
#include<interpreter.h>
#include<string>
#include<functional>

struct dbg_window{
    static const std::string imgui_win_id;
    static void hook(interpreter_t& interp);
    static void draw();
    static void reset_disasm();
    static float size_x,size_y;
protected:
    static void draw_reg_subwindow();
    static void draw_disasm_subwindow();
    static void disassemble(uint16_t adr=0);
    static uint16_t disasm_pos;
    static std::reference_wrapper<interpreter_t> interpreter;
    static interpreter_t placeholder;
    static inline size_t disasm_entries_count{30};
    static inline uint16_t upper_viewable_rom_address{0x7FFF};
};