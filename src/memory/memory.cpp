#include<memory/memory.h>
#include<fstream>
#include<iostream>
#include<gameboy.h>

uint8_t& memory_t::parse_address(uint16_t adr){
    switch(adr){
    case 0x0000 ... 0x3FFF: return rom1[adr];
    case 0x4000 ... 0x7FFF: return rom2.get()[adr-0x4000];
    case 0x8000 ... 0x9FFF: return vram[adr-0x8000];
    case 0xA000 ... 0xBFFF: return ram_banks.get()[adr-0xA000];
    case 0xC000 ... 0xCFFF: return wram[adr-0xC000];
    case 0xD000 ... 0xDFFF: return wram_banks.get()[adr-0xD000];
    case 0xE000 ... 0xEFFF: return wram[adr-0xE000];                //  mirror
    case 0xF000 ... 0xFDFF: return wram_banks.get()[adr-0xF000];    //  mirror
    case 0xFE00 ... 0xFE9F: return oam[adr-0xFE00];
    case 0xFEA0 ... 0xFEFF: return illegal[adr-0xFEA0];
    case 0xFF00 ... 0xFF7F: return io_regs[adr-0xFF00];
    case 0xFF80 ... 0xFFFE: return hram[adr-0xFF80];
    }
    return ie;
}

uint8_t memory_t::read(uint16_t adr){
#ifdef __DEBUG__
    if(dbg_read_breakpoints.size() > 0){
        if(dbg_read_breakpoints.contains(adr)){
            if(dbg_read_breakpoints[adr] && dbg_read_breakpoint_callbk)
                dbg_read_breakpoint_callbk(adr);
        }
    }
#endif 
    switch(adr){
    case 0xFF04:    return ((div_timestamp+gb->scheduler.get_cycles())/256);
                    break;
    case 0x0100:    if(boot_rom_bound) unbind_boot_rom();
    default:        return parse_address(adr);
    }
}

void memory_t::write(uint16_t adr, uint8_t val){
#ifdef __DEBUG__
    if(adr == 0xFF01){
        std::cout << val;
    }
    if(dbg_write_breakpoints.size() > 0){
        if(dbg_write_breakpoints.contains(adr)){
            if(dbg_write_breakpoints[adr] && dbg_write_breakpoint_callbk)
                dbg_write_breakpoint_callbk(adr, val);
        }
    }
#endif
    switch(adr){
    case 0x0000 ... 0x7FFF: on_rom_write(adr);          
                            break;
    case 0xFF04:            div_timestamp = gb->scheduler.get_cycles(); 
                            break;
    case 0xFF0F:            gb->scheduler.add_event({1, {[&](){ gb->handle_interrupts(); }, scheduler_event::IE_WRITE}});
                            parse_address(adr) = val;
                            break;
    case 0xFFFF:            gb->scheduler.add_event({1, {[&](){ gb->handle_interrupts(); }, scheduler_event::IE_WRITE}});
    default:                parse_address(adr) = val;
    }
}

uint8_t memory_t::debug_read(uint16_t adr){ //  doesn't unbind boot rom.
    return parse_address(adr);
}

void memory_t::debug_write(uint16_t adr, uint8_t val){
    parse_address(adr) = val;
}

void memory_t::load_rom(const std::string& path){
    std::ifstream stream{path, std::ios::binary};
    if(!stream)
        throw std::runtime_error("unable to locate rom");
    rom_path = path;
    stream.read(reinterpret_cast<char*>(rom1.data()), rom1.size());
    parse_rom_info();
    std::vector<rom_bank_t> banks_vec;
    banks_vec.reserve((2<<info.rom_size)-1);
    for(rom_bank_t bank; !stream.eof(); banks_vec.push_back(bank),bank={})
        stream.read(reinterpret_cast<char*>(bank.data()), bank.size());
    if(banks_vec.size())
        banks_vec.pop_back();
    rom2.copy_from_vec(banks_vec);
    //  binds the boot rom to rom but saves the unbinded rom to another array.
    bind_boot_rom();
}

void memory_t::bind_boot_rom(){
    boot_rom_bound = true;
    std::copy(rom1.begin(), rom1.end(), unbinded_rom.begin());
    std::ifstream boot_rom_stream{"roms/dmg_boot.bin", std::ios::binary};
    if(!boot_rom_stream)
        throw std::runtime_error("unable to locate boot rom file!");
    boot_rom_stream.read(reinterpret_cast<char*>(rom1.data()),256);
}

void memory_t::unbind_boot_rom(){
    boot_rom_bound = false;
    std::copy(unbinded_rom.begin(), unbinded_rom.end(), rom1.begin());
#ifdef __DEBUG__
        if(dbg_unbind_bootrom_callbk)
            dbg_unbind_bootrom_callbk();
#endif
}

void memory_t::on_rom_write(uint16_t adr){

}

void memory_t::parse_rom_info(){
    std::copy(&rom1[0x0134], &rom1[0x014F], reinterpret_cast<uint8_t*>(&info));
}