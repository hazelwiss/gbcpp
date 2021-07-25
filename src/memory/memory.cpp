#include<memory/memory.h>
#include<fstream>
#include<iostream>
#include<gameboy.h>

enum class interrupt_bit{
    VBLANK      = 0,
    LCD_STAT    = 0b10,
    TIMER       = 0b100,
    SERIAL      = 0b1000,
    JOYPAD      = 0b10000
};

uint8_t memory_t::read(uint16_t adr){
#ifdef __DEBUG__
    if(dbg_read_breakpoints.size() > 0){
        if(dbg_read_breakpoints.contains(adr)){
            if(dbg_read_breakpoints[adr] && dbg_read_breakpoint_callbk)
                dbg_read_breakpoint_callbk(adr);
        }
    }
#endif 
    if(adr < 0x8000){
        if(boot_rom_bound && adr == 0x0100)
            unbind_boot_rom();
        return mbc->read_rom(adr);
    } else{
        switch (adr){
        case 0x8000 ... 0x9FFF: return mbc->read_vram(adr-0x8000);
        case 0xA000 ... 0xBFFF: return mbc->read_ram(adr-0xA000);
        case 0xC000 ... 0xCFFF: return wram[adr-0xC000];
        case 0xD000 ... 0xDFFF: return mbc->read_wram(adr-0xD000);
        case 0xE000 ... 0xEFFF: return wram[adr-0xE000];
        case 0xF000 ... 0xFDFF: return mbc->read_wram(adr-0xF000);
        case 0xFE00 ... 0xFE9F: return oam[adr-0xFE00];
        case 0xFEA0 ... 0xFEFF: return illegal[adr-0xFEA0];
        case 0xFF00 ... 0xFF7F: return read_io(adr);
        case 0xFF80 ... 0xFFFE: return hram[adr-0xFF80];
        }
        return ie;
    }
}

uint8_t memory_t::read_io(uint16_t adr){
    switch(adr){
    case 0xFF04: return ((div_timestamp+gb->scheduler.get_cycles())/256);   //  DIV
    }
    return io_regs[adr-0xFF00];
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
    if(adr < 0x8000){
        mbc->rom_write(adr, val);
    } else{
        switch (adr){
        case 0x8000 ... 0x9FFF: mbc->write_vram(adr-0x8000, val);          break;
        case 0xA000 ... 0xBFFF: mbc->write_ram(adr-0xA000, val);           break;
        case 0xC000 ... 0xCFFF: wram[adr-0xC000]                    = val; break;
        case 0xD000 ... 0xDFFF: mbc->write_wram(adr-0xD000, val);          break;
        case 0xE000 ... 0xEFFF: wram[adr-0xE000]                    = val; break;
        case 0xF000 ... 0xFDFF: mbc->write_wram(adr-0xF000, val);          break;
        case 0xFE00 ... 0xFE9F: oam[adr-0xFE00]                     = val; break;
        case 0xFEA0 ... 0xFEFF: illegal[adr-0xFEA0]                 = val; break;
        case 0xFF00 ... 0xFF7F: write_io(adr, val);                        break;
        case 0xFF80 ... 0xFFFE: hram[adr-0xFF80]                    = val; break;
        default:                gb->scheduler.add_event(1, {[&](){ gb->handle_interrupts(); }, scheduler_event::IE_WRITE});
                                ie = val;
        }
    }
}

void memory_t::write_io(uint16_t adr, uint8_t val){
    switch(adr){
    case 0xFF04:    //  DIV
        div_timestamp = gb->scheduler.get_cycles();
        break;
    case 0xFF06:    //  TMA
    case 0xFF07:    //  TAC
        io_regs[adr-0xFF00] = val;
        if(read(0xFF07)&0b100){
            auto lambda = [&](){
                io_regs[0x0F] |= (uint8_t)interrupt_bit::TIMER;
                gb->handle_interrupts();
                return;
            };
            switch(read(0xFF07)&0b11){
            case 0b00:
                gb->scheduler.add_event((0x100-read(0xFF05))*1024, {lambda, scheduler_event::TIMER});
                break;
            case 0b01:
                gb->scheduler.add_event((0x100-read(0xFF05))*16, {lambda, scheduler_event::TIMER});
                break;
            case 0b10:
                gb->scheduler.add_event((0x100-read(0xFF05))*64, {lambda, scheduler_event::TIMER});
                break;
            case 0b11:
                gb->scheduler.add_event((0x100-read(0xFF05))*256, {lambda, scheduler_event::TIMER});
                break;
            }    
        }
        return;
    case IF_ADR:
        gb->scheduler.add_event(1, {[&](){ gb->handle_interrupts(); }, scheduler_event::IF_WRITE});
    }
    io_regs[adr-0xFF00] = val;
}

uint8_t memory_t::debug_read(uint16_t adr){ //  doesn't unbind boot rom.
    switch (adr){
    case 0x0000 ... 0x7FFF: return mbc->read_rom(adr);
    case 0x8000 ... 0x9FFF: return mbc->read_vram(adr-0x8000);
    case 0xA000 ... 0xBFFF: return mbc->read_ram(adr-0xA000);
    case 0xC000 ... 0xCFFF: return wram[adr-0xC000];
    case 0xD000 ... 0xDFFF: return mbc->read_wram(adr-0xD000);
    case 0xE000 ... 0xEFFF: return wram[adr-0xE000];
    case 0xF000 ... 0xFDFF: return mbc->read_wram(adr-0xF000);
    case 0xFE00 ... 0xFE9F: return oam[adr-0xFE00];
    case 0xFEA0 ... 0xFEFF: return illegal[adr-0xFEA0];
    case 0xFF00 ... 0xFF7F: return io_regs[adr-0xFF00];
    case 0xFF80 ... 0xFFFE: return hram[adr-0xFF80];
    }
    return ie;
}

void memory_t::load_rom(const std::string& path){
    rom_path = path;
    std::ifstream stream{path, std::ios::binary|std::ios::ate};
    if(!stream)
        throw std::runtime_error("unable to locate rom");
    std::vector<char> rom_data(stream.tellg());
    stream.seekg(std::ios::beg);
    stream.read(rom_data.data(), rom_data.size());
    allocate_mbc_type(rom_data[0x0147]);
    mbc->load_rom(rom_data);
    bind_boot_rom();
}

void memory_t::bind_boot_rom(){
    boot_rom_bound = true;
    mbc->strap_boot_rom();
}

void memory_t::unbind_boot_rom(){
    if(boot_rom_bound)
        mbc->unstrap_boot_rom();
    boot_rom_bound = false;
#ifdef __DEBUG__
    if(dbg_unbind_bootrom_callbk)
        dbg_unbind_bootrom_callbk();
#endif
}

void memory_t::allocate_mbc_type(uint8_t mbc_type){
    switch(mbc_type){
    case 0x00:
        mbc = std::make_unique<mbc_none_t>();
        break;
    case 0x01:
        mbc = std::make_unique<mbc1_t>();
        break;
    }
}

void mbc_t::load_rom(const std::vector<char>& rom_data){
    std::copy(&rom_data[0x0134], &rom_data[0x014F], reinterpret_cast<uint8_t*>(&info));
    std::copy(rom_data.begin(), rom_data.begin()+rom1.size(), rom1.data());
    std::vector<rom_bank_t> banks_vec;
    banks_vec.reserve((2<<info.rom_size)-1);
    auto iter = rom_data.begin()+rom1.size();
    for(rom_bank_t bank; iter<rom_data.end(); banks_vec.push_back(bank),bank={},iter+=bank.size())
        std::copy(iter, iter+bank.size(), bank.data());
    rom2.copy_from_vec(banks_vec);
}

void mbc_t::strap_boot_rom(){
    std::copy(rom1.begin(), rom1.end(), unbinded_rom.begin());
    std::ifstream boot_rom_stream{"roms/dmg_boot.bin", std::ios::binary};
    if(!boot_rom_stream)
        throw std::runtime_error("unable to locate boot rom file!");
    boot_rom_stream.read(reinterpret_cast<char*>(rom1.data()),256);
}

void mbc_t::unstrap_boot_rom(){
    std::copy(unbinded_rom.begin(), unbinded_rom.end(), rom1.begin());
}

uint8_t mbc_t::read_rom(uint16_t adr){
    if(adr < 0x4000)
        return rom1[adr];
    return rom2.get()[adr-0x4000];
}

uint8_t mbc_t::read_ram(uint16_t adr){
    return ram_banks.get()[adr];
}

uint8_t mbc_t::read_wram(uint16_t adr){
    return wram_banks.get()[adr];
}

uint8_t mbc_t::read_vram(uint16_t adr){
    return vram_banks.get()[adr];
}

void mbc_t::write_ram(uint16_t adr, uint8_t val){
    ram_banks.get()[adr] = val;
}

void mbc_t::write_wram(uint16_t adr, uint8_t val){
    wram_banks.get()[adr] = val;
}

void mbc_t::write_vram(uint16_t adr, uint8_t val){
    vram_banks.get()[adr] = val;
}

void mbc_none_t::rom_write(uint16_t adr, uint8_t val){
    return;
}

void mbc1_t::rom_write(uint16_t adr, uint8_t val){
    switch(adr){
    case 0x0000 ... 0x1FFF:
        ram_enabled = val == 0x0A;
        break;
    case 0x2000 ... 0x3FFF:
        rom2.set_active((val&0x1F) + (rom2.get_size() > 0x1F)*secondary_bank_index);
        break;
    case 0x4000 ... 0x5FFF:
        if(ram_mode)
            ram_banks.set_active(val&0b11);
        else
            secondary_bank_index = val&0b11;
        break;
    case 0x6000 ... 0x7FFF:
        ram_mode = val == 1;
    }
}