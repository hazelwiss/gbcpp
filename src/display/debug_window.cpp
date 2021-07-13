#include<common_defs.h>
#include<display/debug_window.h>
#include<display/imgui_backends.h>
#include<display/display.h>
#include<disassemble/disassemble.h>
#include<core/instructions.h>
#include<array>
#include<map>

const std::string dbg_window::imgui_win_id = "dbg_debug_window";
float dbg_window::size_x = 1000, dbg_window::size_y = 600;
std::reference_wrapper<interpreter_t> dbg_window::interpreter{main_window::placeholder};
//  disassembler is globally instantiated
disassembler_t disasm;
//  ordered map for storing the disassembled code
std::map<uint16_t, std::string> disassembly;
std::unordered_map<uint16_t, std::string> labels;
uint16_t breakpoint_insert;
std::string search_string;
constexpr size_t search_str_size = 256;

void dbg_window::hook(interpreter_t& interp){
    interpreter = interp;
    disasm = {};
    disassembly = {};
    labels = {};
    disassemble();
    labels = disasm.get_label_map();
}

void dbg_window::on_pause(){
    reset_disasm();
    disassemble();
    labels = disasm.get_label_map();
    interpreter.get().on_pause();
}

void dbg_window::on_play(){
    interpreter.get().on_unpause();
}

void dbg_window::disassemble(uint16_t adr){
    auto& interp = interpreter.get();
    bool is_cb;
    uint8_t opc = interp.mem.debug_read(adr);
    auto instr = (is_cb=(opc == 0xCB)) ? 
        instr_table::cb_range[interp.mem.debug_read(adr+1)] : 
        instr_table::noncb_range[opc];
    while(!disasm.is_noncb_branch(opc) || is_cb){    //  sees if the instruction is a branch.
        if(!disassembly.contains(adr) && adr < 0xE000){
            if(opc!=0)
                disassembly[adr] = disasm.disassemble(opc, adr, interp.mem.debug_read(adr+1)|(interp.mem.debug_read(adr+2)<<8));
        } else
            return;
        adr+=entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
        opc = interp.mem.debug_read(adr);
        instr = (is_cb=(opc == 0xCB)) ? 
            instr_table::cb_range[interp.mem.debug_read(adr+1)] : 
            instr_table::noncb_range[opc];
        if(!entry_get<CPU_ENTRY::BYTE_LENGTH>(instr))
            return;
    }
    if(!disassembly.contains(adr)){
        disassembly[adr] = disasm.disassemble(opc, 
            adr+entry_get<CPU_ENTRY::BYTE_LENGTH>(instr), interp.mem.debug_read(adr+1)|(interp.mem.debug_read(adr+2)<<8));
    } else 
        return;
    uint16_t offset_adr = adr+entry_get<CPU_ENTRY::BYTE_LENGTH>(instr);
    if(disasm.is_labelifyable(opc)){
        uint16_t imm = interp.mem.debug_read(adr+1)|(interp.mem.debug_read(adr+2)<<8);
        uint16_t branched_adr = disasm.get_branch_results(opc, offset_adr, imm);
        disassemble(branched_adr);
    }
    if(disasm.is_conditional(opc) || disasm.is_call(opc) || opc == 0xE9)
        disassemble(offset_adr);
}

void dbg_window::reset_disasm(){
    disasm = {};
    disassembly = {};
}

void dbg_window::draw(){
    ImGui::SetNextWindowSize({size_x, size_y});
    if(ImGui::Begin(imgui_win_id.c_str(), 
        reinterpret_cast<bool*>(&main_window::enable_debug_window), ImGuiWindowFlags_NoCollapse))
    {
        draw_reg_subwindow();
        ImGui::SameLine();
        draw_disasm_subwindow();
        ImGui::SameLine();
        draw_control_subwindow();
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
            std::array<uint16_t*,6> regs = {
                &interp.cpu.regs.get<RI::AF>(),
                &interp.cpu.regs.get<RI::BC>(),
                &interp.cpu.regs.get<RI::DE>(),
                &interp.cpu.regs.get<RI::HL>(),
                &interp.cpu.regs.get<RI::SP>(),
                &interp.cpu.regs.get<RI::PC>()
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
                ImGui::InputScalar(names[i*2].c_str(), ImGuiDataType_U16, regs[i*2], 
                    nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::TableSetColumnIndex(1);
                ImGui::InputScalar(names[i*2+1].c_str(), ImGuiDataType_U16, regs[i*2+1], 
                    nullptr, nullptr, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
            }
            for(size_t i = 0; i < 2; ++i){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s: %d", names[6+i*2].c_str(), flags[i*2]);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s: %d", names[7+i*2].c_str(), flags[i*2+1]);
            }
            ImGui::EndTable();
        }
        std::string text = "Call Stack";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(text.c_str()).x)/2);
        ImGui::Text(text.c_str());
        if(ImGui::BeginTable(text.c_str(), 1, 0, {0,size_y/4.55f})){
            uint32_t count = interp.call_deque.size();
            for(const auto& entry: interp.call_deque){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d: %04X -> %04X", --count, entry.first, entry.second);
            }
            ImGui::EndTable();
        }
        text = "Recent Instructions";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x-ImGui::CalcTextSize(text.c_str()).x)/2);
        ImGui::Text(text.c_str());
        if(ImGui::BeginTable(text.c_str(), 1)){
            for(const auto entry: interp.recent_instr_deque){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%04X: %s", entry.first, entry.second.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}

void dbg_window::draw_disasm_subwindow(){
    auto& interp = interpreter.get();
    if(ImGui::BeginChild("disassembly", {size_x/2.2f,0}, true)){
        std::array<char, search_str_size>buffer;
        std::fill(buffer.begin(), buffer.end(), 0);
        ImGui::InputText("find:", buffer.data(), search_str_size);
        search_string = buffer.data();
        if(ImGui::BeginTable("disassembly", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY)){
            //  column 0: breakpoints   column 1: address
            //  column 2: mnemonic  
            ImVec4 col_green = {0.5,0.8,0.5,1};
            ImVec4 col_red = {0.8,0.5,0.5,1};
            for(auto& entry: disassembly){
                if(search_string != "" && entry.second.find(search_string) == std::string::npos)
                    continue;
                if(labels.contains(entry.first)){
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextColored(col_green, "%s:", labels[entry.first].c_str());
                }
                ImGui::TableNextRow();
                if(entry.first == interp.cpu.regs.get<RI::PC>()){
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, 0xAAAAAAAA);
                }
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s:", disasm.get_memory_region_string(entry.first).c_str());
                if(interp.cpu.code_breakpoints.contains(entry.first)){
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextColored(col_red, "%s", "B");
                }
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%04X", entry.first);
                ImGui::TableSetColumnIndex(3);
                ImGui::TextColored(col_green, "   %s", entry.second.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
    }   
}

void dbg_window::draw_control_subwindow(){
    auto& interp = interpreter.get();
    if(ImGui::BeginChild("control", {0,0}, true)){
        if(ImGui::ArrowButton("arrow", ImGuiDir_Right)){
            interp.paused = false;
            on_play();
        }
        ImGui::SameLine();
        if(ImGui::Button("||")){
            interp.paused = true;
            on_pause();
        }
        ImGui::SameLine();
        ImGui::PushButtonRepeat(true);
        if(ImGui::Button("->")){
            interp.should_step = true;
        }
        ImGui::PopButtonRepeat();
        ImGui::SameLine();
        if(ImGui::Button("RESET")){
            interp.reset();
        }
        ImGui::SameLine();
        ImGui::Text("fps: %ld", interp.fps.load());
        ImGui::SameLine();
        ImGui::Text("%s", interp.paused ? "paused" : "running");
        //  breakpoints menu.
        ImGui::InputScalar("", ImGuiDataType_U16, &breakpoint_insert, nullptr, nullptr, "%04X", 
            ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::SameLine();
        if(ImGui::Button("remove")){
            interp.cpu.code_breakpoints.erase(breakpoint_insert);
            interp.mem.write_breakpoints.erase(breakpoint_insert);
            interp.mem.read_breakpoints.erase(breakpoint_insert);
        }
        ImGui::Text("breakpoint: ");
        ImGui::SameLine();
        if(ImGui::Button("code")){
            interp.cpu.code_breakpoints[breakpoint_insert] = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("write")){
            interp.mem.write_breakpoints[breakpoint_insert] = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("read")){
            interp.mem.read_breakpoints[breakpoint_insert] = true;
        }
        if(ImGui::BeginChild("breakpoints",{0,0},true)){
            ImGui::Text("code breakpoints:");
            if(ImGui::BeginTable("code breakpoints", 1)){
                for(auto entry: interp.cpu.code_breakpoints){
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    bool contains = disassembly.contains(entry.first);
                    ImGui::Text("%04X -> [%s]", entry.first, contains ?  disassembly[entry.first].c_str() : "");
                }
                ImGui::EndTable();
            }
            ImGui::Text("\nwrite breakpoints:");
            if(ImGui::BeginTable("write breakpoints", 1)){
                for(auto entry: interp.mem.write_breakpoints){
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%04X", entry.first);
                }
                ImGui::EndTable();
            }
            ImGui::Text("\nread breakpoints:");
            if(ImGui::BeginTable("read breakpoints", 1)){
                for(auto entry: interp.mem.read_breakpoints){
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%04X", entry.first);
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
}
