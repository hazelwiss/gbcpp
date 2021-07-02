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
std::map<uint16_t, std::string> labels;

void dbg_window::hook(interpreter_t& interp){
    interpreter = interp;
    disassemble();
}

void dbg_window::disassemble(bool should_branch, uint16_t adr){
    return;
    auto& interp = interpreter.get();
    bool is_cb;
    auto instr = (is_cb=(interp.mem.read(adr) == 0xCB)) ? 
        instr_table::cb_range[interp.mem.read(adr+1)] : 
        instr_table::noncb_range[interp.mem.read(adr)];
    auto ticks = entry_get<CPU_ENTRY::TICKS>(instr);
    while(ticks.first != ticks.second){ //  sees if the instruction is a branch.
        //if(adr+entry_get<CPU_ENTRY::BYTE_LENGTH> > 0xFFFF)
        //    return;
        if(!disassembly.contains(adr)){
            disassembly[adr] = is_cb ? 
                disasm.disassemble_cb(adr) : 
                disasm.disassemble(adr, interp.mem.read(adr+1)|(interp.mem.read(adr+2)));
        }
        adr+=entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
        instr = (is_cb=(interp.mem.read(adr) == 0xCB)) ? 
            instr_table::cb_range[interp.mem.read(adr+1)] : 
            instr_table::noncb_range[interp.mem.read(adr)];
        ticks = entry_get<CPU_ENTRY::TICKS>(instr);
    }
    /*
        work on this!
    */
    disassemble(true, adr);
    disassemble(false, adr);
}

void dbg_window::reset_disasm(){
    disasm = disassembler_t{};
    disassembly = std::map<uint16_t, std::string> {};
}

void dbg_window::draw(){
    auto& interp = interpreter.get();
    ImGui::SetNextWindowSize({size_x, size_y});
    if(ImGui::Begin(imgui_win_id.c_str(), &main_window::enable_debug_window)){
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
            ImGui::SameLine();
            if(ImGui::BeginChild("disassembly", {size_x/1.6f,0}, true)){
                //  memory pos slider
                uint16_t step = 1, fast_step = step;
                if(ImGui::InputScalar("", ImGuiDataType_U16, &disasm_pos, &step, &fast_step, "%04X", ImGuiInputTextFlags_CharsHexadecimal)){
                    if(disasm_pos > upper_viewable_rom_address)
                        disasm_pos = upper_viewable_rom_address;
                }
                //  disassembly/breakpoints
                if(ImGui::BeginTable("disassembly", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY)){
                    //  column 0: breakpoints   column 1: address
                    //  column 2: mnemonic     
                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }
        }        
        ImGui::End();
    }
}