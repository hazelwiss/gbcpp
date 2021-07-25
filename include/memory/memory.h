#pragma once
#include<common_defs.h>
#include<memory/cart.h>
#include<vector>
#include<string>
#include<functional>
#include<stdexcept>
#include<memory>

struct gameboy_t;

template<typename t>
struct banks_t{
    void alloc(size_t size){ banks = std::vector<t>(size); }
    void copy_from_vec(const std::vector<t>& vec){ banks = vec; }
    t& get(){ return banks[index]; }
    void set_active(size_t index){ index = index; }
    size_t get_size(){ return banks.size(); }
protected:
    size_t index{0};
    std::vector<t> banks{1};
};

struct mbc_t{
    virtual void rom_write(uint16_t adr, uint8_t val) = 0;
    uint8_t read_rom(uint16_t adr);
    uint8_t read_ram(uint16_t adr);
    uint8_t read_wram(uint16_t adr);
    uint8_t read_vram(uint16_t adr);
    void write_ram(uint16_t adr, uint8_t val);
    void write_wram(uint16_t adr, uint8_t val);
    void write_vram(uint16_t adr, uint8_t val);
    void strap_boot_rom();
    void unstrap_boot_rom();
    void load_rom(const std::vector<char>& rom_data);
protected:
    rom_bank_t rom1, unbinded_rom{0};
    banks_t<rom_bank_t> rom2;
    banks_t<ram_bank_t> ram_banks;
    banks_t<std::array<uint8_t,0x1000>> wram_banks;
    banks_t<std::array<uint8_t,0x2000>> vram_banks;
    rom_info_t info;
};

struct mbc_none_t: mbc_t{
    void rom_write(uint16_t adr, uint8_t val) final;
};

struct mbc1_t: mbc_t{
    void rom_write(uint16_t adr, uint8_t val) final;
    bool ram_enabled{false};
    size_t secondary_bank_index{0};
    bool ram_mode{false};
};

static inline const uint16_t IE_ADR = 0xFFFF;
static inline const uint16_t IF_ADR = 0xFF0F;

struct memory_t{
    memory_t(gameboy_t* gb): gb{gb} { 
        if(gb == nullptr)
            throw std::runtime_error("invalid pointer in memory constructor.");
        write(0xFF44, 0x90);
    }
    uint8_t read(uint16_t adr);
    void write(uint16_t adr, uint8_t val);
    void load_rom(const std::string& path);
    const std::string& get_rom_path(){ return rom_path; }
    gameboy_t* gb;
    //  debug members and callbacks
    uint8_t debug_read(uint16_t adr); //  reads for the debugger to use.
    void debug_write(uint16_t adr, uint8_t val);
    std::function<void()> dbg_unbind_bootrom_callbk;
    std::function<void(uint16_t)> dbg_read_breakpoint_callbk;
    std::function<void(uint16_t, uint8_t)> dbg_write_breakpoint_callbk;
    std::unordered_map<uint16_t, bool> dbg_read_breakpoints;
    std::unordered_map<uint16_t, bool> dbg_write_breakpoints;
protected:
    void allocate_mbc_type(uint8_t);
    void bind_boot_rom();
    void unbind_boot_rom();
    void write_io(uint16_t adr, uint8_t val);
    uint8_t read_io(uint16_t adr);
    bool boot_rom_bound{false};
    std::unique_ptr<mbc_t> mbc;
    std::array<uint8_t,0x1000> wram{0};
    std::array<uint8_t,0x9F> oam{0};
    std::array<uint8_t,0x7F> io_regs{0};
    std::array<uint8_t,0x5F> illegal{0};
    std::array<uint8_t,0x7E> hram{0};
    uint8_t ie{0};
    std::string rom_path;
    size_t div_timestamp{0};
};