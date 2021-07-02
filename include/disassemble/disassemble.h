/*
    Decodes binary instructions to string mnemonics for disassembly.
*/
#pragma once
#include<string>
#include<array>
#include<unordered_map>

struct disassembler_t{
    std::string disassemble(uint8_t opc, uint16_t imm);
    std::string disassemble_cb(uint8_t opc);
    const auto& get_label_map(){ return labels; }
protected:
    std::unordered_map<uint16_t, std::string> labels;
    std::string labelify_opc(uint8_t opc, uint16_t imm);
    static std::array<const char*,256> noncb_mnemonic;
    static std::array<const char*,256> cb_mnemonic;
    static std::array<bool,256> labelify;
};