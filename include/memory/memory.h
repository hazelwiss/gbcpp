#pragma once
#include<common_defs.h>
#include<memory/cart.h>
#include<vector>
#include<string>
#include<functional>

template<typename t>
struct banks_t{
    void alloc(size_t size){ banks = std::vector<t>(size); }
    void copy_from_vec(const std::vector<t>& vec){ banks = vec; }
    t& get(){ return banks[index]; }
    void set_active(size_t index){ index = index; }
protected:
    size_t index{0};
    std::vector<t> banks{1};
};

struct mbc_t{
    //  keep rom banks here, and also ram banks/external ram
    //  probably put this in another header... will use inheritance later to determine mbc behaviour.
};

struct memory_t{
    uint8_t read(uint16_t adr);
    void write(uint16_t adr, uint8_t val);
    void load_rom(std::string path);
    void bind_unbind_bootrom_callbk(std::function<void()> arg){ unbind_bootrom_callbk=arg; };
protected:
    void bind_boot_rom();
    void unbind_boot_rom();
    void on_rom_write(uint16_t adr);
    void parse_rom_info();
    bool boot_rom_bound{false};
    rom_info_t info;
    rom_bank_t rom1, unbinded_rom;
    banks_t<rom_bank_t> rom2;
    std::array<uint8_t,0x2000> vram;
    banks_t<ram_bank_t> ram_banks;
    std::array<uint8_t,0x1000> wram;
    banks_t<std::array<uint8_t,0x1000>> wram_banks;
    //  echo ram???
    std::array<uint8_t,0x9F> oam;
    std::array<uint8_t,0x7F> io_regs;
    std::array<uint8_t,0x7E> hram;
    uint8_t ie;
    std::function<void()> unbind_bootrom_callbk;
};