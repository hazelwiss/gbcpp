#include<display/debug_window.h>
#include<display/imgui_backends.h>
#include<display/display.h>
#include<disassemble/disassemble.h>
#include<core/instructions.h>
#include<array>
#include<map>

const std::string dbg_window::imgui_win_id = "dbg_debug_window";
float dbg_window::size_x = 1000, dbg_window::size_y = 600;
interpreter_t dbg_window::placeholder{};
std::reference_wrapper<interpreter_t> dbg_window::interpreter{placeholder};
uint16_t dbg_window::disasm_pos = 0;
//  disassembler is globally instantiated
disassembler_t disasm;
//  ordered map for storing the disassembled code
std::map<uint16_t, std::string> disassembly;
std::unordered_map<uint16_t, std::string> labels;

void dbg_window::hook(interpreter_t& interp){
    interpreter = interp;
    disassemble();
    labels = disasm.get_label_map();
}

void dbg_window::disassemble(uint16_t adr){
    auto& interp = interpreter.get();
    bool is_cb;
    uint8_t opc = interp.mem.read(adr);
    auto instr = (is_cb=(opc == 0xCB)) ? 
        instr_table::cb_range[interp.mem.read(adr+1)] : 
        instr_table::noncb_range[opc];
    auto ticks = entry_get<CPU_ENTRY::TICKS>(instr);
    if(adr+entry_get<CPU_ENTRY::BYTE_LENGTH>(instr) >= 0x8000)
        return;
    else if(!entry_get<CPU_ENTRY::BYTE_LENGTH>(instr))
        return;
    while(!disasm.is_noncb_branch(opc) || is_cb){    //  sees if the instruction is a branch.
        if(!disassembly.contains(adr)){
            if(opc != 0){
                disassembly[adr] = is_cb ? 
                    disasm.disassemble_cb(interp.mem.read(adr+1)) : 
                    disasm.disassemble(opc, adr, interp.mem.read(adr+1)|(interp.mem.read(adr+2)<<8));
            }
        } else
            return;
        adr+=entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
        opc = interp.mem.read(adr);
        instr = (is_cb=(opc == 0xCB)) ? 
            instr_table::cb_range[interp.mem.read(adr+1)] : 
            instr_table::noncb_range[opc];
        ticks = entry_get<CPU_ENTRY::TICKS>(instr);
        if(adr+entry_get<CPU_ENTRY::BYTE_LENGTH>(instr) >= 0x8000)
            return;
        else if(!entry_get<CPU_ENTRY::BYTE_LENGTH>(instr))
            return;
    }
    if(!disassembly.contains(adr)){
        disassembly[adr] = disasm.disassemble(interp.mem.read(adr), 
            adr+entry_get<CPU_ENTRY::BYTE_LENGTH>(instr), interp.mem.read(adr+1)|(interp.mem.read(adr+2)<<8));
    } else 
        return;
    uint16_t imm = interp.mem.read(adr+1)|(interp.mem.read(adr+2)<<8);
    adr += entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
    uint16_t branched_adr = disasm.get_branch_results(opc, adr, imm);
    disassemble(branched_adr);
    disassemble(adr);
}

void dbg_window::reset_disasm(){
    disasm = disassembler_t{};
    disassembly = std::map<uint16_t, std::string> {};
}

void dbg_window::draw(){
    ImGui::SetNextWindowSize({size_x, size_y});
    if(ImGui::Begin(imgui_win_id.c_str(), &main_window::enable_debug_window)){
        draw_reg_subwindow();
        ImGui::SameLine();
        draw_disasm_subwindow();
        ImGui::End();
    }
}

void dbg_window::draw_reg_subwindow(){
    auto& interp = interpreter.get();
    if(ImGui::BeginChild("regs and callstack", {size_x/5.5f,0}, true)){
        if(ImGui::BeginTable("reg and flag table", 2, 0, {0, size_y/5})){
            std::array<std::string,10> names = {
                "AF", "BC",
                "DE", "HL",
                "SP", "PC",
                "Z",  "N",
                "H",  "C"
            };
            std::array<uint16_t,6> regs = {
                interp.cpu.regs.get<RI::AF>(),
                interp.cpu.regs.get<RI::BC>(),
                interp.cpu.regs.get<RI::DE>(),
                interp.cpu.regs.get<RI::HL>(),
                interp.cpu.regs.get<RI::SP>(),
                interp.cpu.regs.get<RI::PC>()
            };
            std::array<bool,4> flags = {
                interp.cpu.regs.get_flag<FI::Z>(),
                interp.cpu.regs.get_flag<FI::N>(),
                interp.cpu.regs.get_flag<FI::H>(),
                interp.cpu.regs.get_flag<FI::C>()
            };
            for(size_t i = 0; i < 3; ++i){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::InputScalar(names[i*2].c_str(), ImGuiDataType_U16, &regs[i*2], 
                    nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::TableSetColumnIndex(1);
                ImGui::InputScalar(names[i*2+1].c_str(), ImGuiDataType_U16, &regs[i*2+1], 
                    nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
            }
            for(size_t i = 0; i < 2; ++i){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::InputScalar(names[6+i*2].c_str(), ImGuiDataType_U8, &flags[i]);
                ImGui::TableSetColumnIndex(1);
                ImGui::InputScalar(names[7+i*2].c_str(), ImGuiDataType_U8, &flags[i+1]);
            }
            ImGui::EndTable();
        }
        std::string text = "Call Stack";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(text.c_str()).x)/2);
        ImGui::Text("Call Stack");
        if(ImGui::BeginTable(text.c_str(), 2)){
            //  do call stack here
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}

void dbg_window::draw_disasm_subwindow(){
    if(ImGui::BeginChild("disassembly", {size_x/1.6f,0}, true)){
        //  memory pos slider
        //uint16_t step = 1, fast_step = step;
        //if(ImGui::InputScalar("", ImGuiDataType_U16, &disasm_pos, &step, &fast_step, "%04X", ImGuiInputTextFlags_CharsHexadecimal))
        //    disasm_pos = disasm_pos > upper_viewable_rom_address ? upper_viewable_rom_address : disasm_pos;
        //  disassembly/breakpoints
        if(ImGui::BeginTable("disassembly", 3, ImGuiTableFlags_SizingFixedFit)){
            //  column 0: breakpoints   column 1: address
            //  column 2: mnemonic  
            ImVec4 col = {0.5,0.8,0.5,1};
            for(auto& entry: disassembly){
                if(labels.contains(entry.first)){
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(col, "  %s:", labels[entry.first].c_str());
                }
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%04X", entry.first);
                ImGui::TableSetColumnIndex(2);
                ImGui::TextColored(col, "%s", entry.second.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }   
}