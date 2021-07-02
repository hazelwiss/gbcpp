#pragma once
#include<common_defs.h>
#include<string>
#include<array>

using rom_bank_t = std::array<uint8_t,0x4000>;
using ram_bank_t = std::array<uint8_t,0x2000>;


struct __attribute__((packed)) rom_info_t{
    std::array<char,11> title;
    uint32_t manufacturer_code;
    uint8_t cgb_flag;
    uint16_t new_license_code;
    uint8_t sgb_flag;
    uint8_t cart_type;
    uint8_t rom_size;
    uint8_t ram_size;
    uint8_t dest_code;
    uint8_t old_license_code;
    uint8_t mask_rom_vers;
    uint8_t header_checksum;
    uint16_t global_checksum;
};