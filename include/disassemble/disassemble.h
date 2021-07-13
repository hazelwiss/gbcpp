/*
    Decodes binary instructions to string mnemonics for disassembly.
*/
#pragma once
#include<string>
#include<array>
#include<unordered_map>

enum class memory_type{
    ROM0,
    ROM1,
    VRAM,
    RAM,
    WRAM,
    MIRROR,
    OAM,
    ILLEGAL,
    IO,
    HRAM
};

struct disassembler_t{
    std::string disassemble(uint8_t opc, uint16_t offset, uint16_t imm);
    const auto& get_label_map(){ return labels; }
    static bool is_noncb_branch(uint8_t opc);
    static uint16_t get_branch_results(uint8_t opc, uint16_t offset, uint16_t imm);
    static bool is_ret(uint8_t opc);
    static bool is_call(uint8_t opc);
    static bool is_conditional(uint8_t opc);
    static bool is_labelifyable(uint8_t opc);
    memory_type get_memory_type(uint16_t adr);
    static std::string get_memory_region_string(uint16_t adr);
protected:
    std::unordered_map<uint16_t, std::string> labels;
    std::string labelify_opc(uint8_t opc, uint16_t offset, uint16_t imm);
    static std::array<const char*,256> noncb_mnemonic;
    static std::array<const char*,256> cb_mnemonic;
    static std::array<bool,256> labelify_table;
    static std::array<bool,256> call_table;
    static std::array<bool,256> ret_table;
    static std::array<bool,256> conditional_table;
    static std::array<bool,256> branch_table;
};