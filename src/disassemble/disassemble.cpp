#include<disassemble/disassemble.h>
#include<stdexcept>
#include<sstream>
#include<iomanip>
#include<cstring>

std::array<const char*,256> disassembler_t::noncb_mnemonic = {
    "nop",
    "ld bc, %2",
    "ld (bc), a",
    "inc bc",
    "inc b",
    "dec b",
    "ld b, %1",
    "rlca",
    "ld (%2), sp",
    "add hl, bc",
    "ld a, (bc)",
    "dec bc",
    "inc c",
    "dec c",
    "ld c, %1",
    "rrca",
    "stop",
    "ld de, %2",
    "ld (de), a",
    "inc de",
    "inc d",
    "dec d",
    "ld d, %1",
    "rla",
    "jr %-",
    "add hl, de",
    "ld a, (de)",
    "dec de",
    "inc e",
    "dec e",
    "ld e, %1",
    "rra",
    "jr nz, %-",
    "ld hl, %2",
    "ld (hl+), a",
    "inc hl",
    "inc h",
    "dec h",
    "ld h, %1",
    "daa",
    "jr z, %-",
    "add hl, hl",
    "ld a, (hl+)",
    "dec hl",
    "inc l",
    "dec l",
    "ld l, %1",
    "cpl",
    "jr nc, %-",
    "ld sp, %2",
    "ld (hl-), a",
    "inc sp",
    "inc (hl)",
    "dec (hl)",
    "ld (hl), %1",
    "scf",
    "jr c, %-",
    "add hl, sp",
    "ld a, (hl-)",
    "dec sp",
    "inc a",
    "dec a",
    "ld a, %1",
    "ccf",
    "ld b, b",
    "ld b, c",
    "ld b, d",
    "ld b, e",
    "ld b, h",
    "ld b, l",
    "ld b, (hl)",
    "ld b, a",
    "ld c, b",
    "ld c, c",
    "ld c, d",
    "ld c, e",
    "ld c, h",
    "ld c, l",
    "ld c, (hl)",
    "ld c, a",
    "ld d, b",
    "ld d, c",
    "ld d, d",
    "ld d, e",
    "ld d, h",
    "ld d, l",
    "ld d, (hl)",
    "ld d, a",
    "ld e, b",
    "ld e, c",
    "ld e, d",
    "ld e, e",
    "ld e, h",
    "ld e, l",
    "ld e, (hl)",
    "ld e, a",
    "ld h, b",
    "ld h, c",
    "ld h, d",
    "ld h, e",
    "ld h, h",
    "ld h, l",
    "ld h, (hl)",
    "ld h, a",
    "ld l, b",
    "ld l, c",
    "ld l, d",
    "ld l, e",
    "ld l, h",
    "ld l, l",
    "ld l, (hl)",
    "ld l, a",
    "ld (hl), b",
    "ld (hl), c",
    "ld (hl), d",
    "ld (hl), e",
    "ld (hl), h",
    "ld (hl), l",
    "halt",
    "ld (hl), a",
    "ld a, b",
    "ld a, c",
    "ld a, d",
    "ld a, e",
    "ld a, h",
    "ld a, l",
    "ld a, (hl)",
    "ld a, a",
    "add b",
    "add c",
    "add d",
    "add e",
    "add h",
    "add l",
    "add (hl)",
    "add a",
    "adc b",
    "adc c",
    "adc d",
    "adc e",
    "adc h",
    "adc l",
    "adc (hl)",
    "adc a",
    "sub b",
    "sub c",
    "sub d",
    "sub e",
    "sub h",
    "sub l",
    "sub (hl)",
    "sub a",
    "sbc b",
    "sbc c",
    "sbc d",
    "sbc e",
    "sbc h",
    "sbc l",
    "sbc (hl)",
    "sbc a",
    "and b",
    "and c",
    "and d",
    "and e",
    "and h",
    "and l",
    "and (hl)",
    "and a",
    "xor b",
    "xor c",
    "xor d",
    "xor e",
    "xor h",
    "xor l",
    "xor (hl)",
    "xor a",
    "or a, b",
    "or a, c",
    "or a, d",
    "or a, e",
    "or a, h",
    "or a, l",
    "or a, (hl)",
    "or a, a",
    "cp b",
    "cp c",
    "cp d",
    "cp e",
    "cp h",
    "cp l",
    "cp (hl)",
    "cp a",
    "ret nz",
    "pop bc",
    "jp nz, %2",
    "jp %2",
    "call nz, %2",
    "push bc",
    "add %1",
    "rst 00h",
    "ret z",
    "ret",
    "jp z, %2",
    "prefix cb",
    "call z, %2",
    "call %2",
    "adc %1",
    "rst 08h",
    "ret nz",
    "pop de",
    "jp nc, %2",
    "n/a",
    "call nc, %2",
    "push de",
    "sub %1",
    "rst 10h",
    "ret c",
    "reti",
    "jp c, %2",
    "n/a",
    "call c, %2",
    "n/a",
    "sbc %1",
    "rst 18h",
    "ldh (%1), a",
    "pop hl",
    "ldh (c), a",
    "n/a",
    "n/a",
    "push hl",
    "and %1",
    "rst 20h",
    "add sp, %-",
    "jp hl",
    "ld (%2),a",
    "n/a",
    "n/a",
    "n/a",
    "xor %1",
    "rst 28h",
    "ldh a, (%1)",
    "pop af",
    "ldh a, (c)",
    "di",
    "n/a",
    "push af",
    "or a, %1",
    "rst 30h",
    "ld hl, sp+%-",
    "ld sp, hl",
    "ld a, (%2)",
    "EI",
    "n/a",
    "n/a",
    "cp %1",
    "rst 38h"
};
std::array<const char*,256> disassembler_t::cb_mnemonic = {
    "rlc b",
    "rlc c",
    "rlc d",
    "rlc e",
    "rlc h",
    "rlc l",
    "rlc (hl)",
    "rlc a",
    "rrc b",
    "rrc c",
    "rrc d",
    "rrc e",
    "rrc h",
    "rrc l",
    "rrc (hl)",
    "rrc a",
    "rl b",
    "rl c",
    "rl d",
    "rl e",
    "rl h",
    "rl l",
    "rl (hl)",
    "rl a",
    "rr b",
    "rr c",
    "rr d",
    "rr e",
    "rr h",
    "rr l",
    "rr (hl)",
    "rr a",
    "sla b",
    "sla c",
    "sla d",
    "sla e",
    "sla h",
    "sla l",
    "sla (hl)",
    "sla a",
    "sra b",
    "sra c",
    "sra d",
    "sra e",
    "sra h",
    "sra l",
    "sra (hl)",
    "sra a",
    "swap b",
    "swap c",
    "swap d",
    "swap e",
    "swap h",
    "swap l",
    "swap (hl)",
    "swap a",
    "srl b",
    "srl c",
    "srl d",
    "srl e",
    "srl h",
    "srl l",
    "srl (hl)",
    "srl a",
    "bit 0, b",
    "bit 0, c",
    "bit 0, d",
    "bit 0, e",
    "bit 0, h",
    "bit 0, l",
    "bit 0, (hl)",
    "bit 0, a",
    "bit 1, b",
    "bit 1, c",
    "bit 1, d",
    "bit 1, e",
    "bit 1, h",
    "bit 1, l",
    "bit 1, (hl)",
    "bit 1, a",
    "bit 2, b",
    "bit 2, c",
    "bit 2, d",
    "bit 2, e",
    "bit 2, h",
    "bit 2, l",
    "bit 2, (hl)",
    "bit 2, a",
    "bit 3, b",
    "bit 3, c",
    "bit 3, d",
    "bit 3, e",
    "bit 3, h",
    "bit 3, l",
    "bit 3, (hl)",
    "bit 3, a",
    "bit 4, b",
    "bit 4, c",
    "bit 4, d",
    "bit 4, e",
    "bit 4, h",
    "bit 4, l",
    "bit 4, (hl)",
    "bit 4, a",
    "bit 5, b",
    "bit 5, c",
    "bit 5, d",
    "bit 5, e",
    "bit 5, h",
    "bit 5, l",
    "bit 5, (hl)",
    "bit 5, a",
    "bit 6, b",
    "bit 6, c",
    "bit 6, d",
    "bit 6, e",
    "bit 6, h",
    "bit 6, l",
    "bit 6, (hl)",
    "bit 6, a",
    "bit 7, b",
    "bit 7, c",
    "bit 7, d",
    "bit 7, e",
    "bit 7, h",
    "bit 7, l",
    "bit 7, (hl)",
    "bit 7, a",
    "res 0, b",
    "res 0, c",
    "res 0, d",
    "res 0, e",
    "res 0, h",
    "res 0, l",
    "res 0, (hl)",
    "res 0, a",
    "res 1, b",
    "res 1, c",
    "res 1, d",
    "res 1, e",
    "res 1, h",
    "res 1, l",
    "res 1, (hl)",
    "res 1, a",
    "res 2, b",
    "res 2, c",
    "res 2, d",
    "res 2, e",
    "res 2, h",
    "res 2, l",
    "res 2, (hl)",
    "res 2, a",
    "res 3, b",
    "res 3, c",
    "res 3, d",
    "res 3, e",
    "res 3, h",
    "res 3, l",
    "res 3, (hl)",
    "res 3, a",
    "res 4, b",
    "res 4, c",
    "res 4, d",
    "res 4, e",
    "res 4, h",
    "res 4, l",
    "res 4, (hl)",
    "res 4, a",
    "res 5, b",
    "res 5, c",
    "res 5, d",
    "res 5, e",
    "res 5, h",
    "res 5, l",
    "res 5, (hl)",
    "res 5, a",
    "res 6, b",
    "res 6, c",
    "res 6, d",
    "res 6, e",
    "res 6, h",
    "res 6, l",
    "res 6, (hl)",
    "res 6, a",
    "res 7, b",
    "res 7, c",
    "res 7, d",
    "res 7, e",
    "res 7, h",
    "res 7, l",
    "res 7, (hl)",
    "res 7, a",
    "set 0, b",
    "set 0, c",
    "set 0, d",
    "set 0, e",
    "set 0, h",
    "set 0, l",
    "set 0, (hl)",
    "set 0, a",
    "set 1, b",
    "set 1, c",
    "set 1, d",
    "set 1, e",
    "set 1, h",
    "set 1, l",
    "set 1, (hl)",
    "set 1, a",
    "set 2, b",
    "set 2, c",
    "set 2, d",
    "set 2, e",
    "set 2, h",
    "set 2, l",
    "set 2, (hl)",
    "set 2, a",
    "set 3, b",
    "set 3, c",
    "set 3, d",
    "set 3, e",
    "set 3, h",
    "set 3, l",
    "set 3, (hl)",
    "set 3, a",
    "set 4, b",
    "set 4, c",
    "set 4, d",
    "set 4, e",
    "set 4, h",
    "set 4, l",
    "set 4, (hl)",
    "set 4, a",
    "set 5, b",
    "set 5, c",
    "set 5, d",
    "set 5, e",
    "set 5, h",
    "set 5, l",
    "set 5, (hl)",
    "set 5, a",
    "set 6, b",
    "set 6, c",
    "set 6, d",
    "set 6, e",
    "set 6, h",
    "set 6, l",
    "set 6, (hl)",
    "set 6, a",
    "set 7, b",
    "set 7, c",
    "set 7, d",
    "set 7, e",
    "set 7, h",
    "set 7, l",
    "set 7, (hl)",
    "set 7, a"
};
std::array<bool,256> disassembler_t::labelify_table = {
//       0     1     2     3          4     5     6     7            8     9     A     B           C     D     E     F       
/*0*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*1*/    false,false,false,false,     false,false,false,false,       true,false,false,false,       false,false,false,false,
/*2*/    true,false,false,false,      false,false,false,false,       true,false,false,false,       false,false,false,false,
/*3*/    true,false,false,false,      false,false,false,false,       true,false,false,false,       false,false,false,false,

/*4*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*5*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*6*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*7*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*8*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*9*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*A*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*B*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*C*/    false,false,true,true,       true,false,false,true,         false,false,true,false,       true,true,false,true,
/*D*/    false,false,true,false,      true,false,false,true,         false,false,true,false,       true,false,false,true,
/*E*/    false,false,false,false,     false,false,false,true,        false,false,false,false,      false,false,false,true,
/*F*/    false,false,false,false,     false,false,false,true,        false,false,false,false,      false,false,false,true
};

std::array<bool,256> disassembler_t::ret_table = {
//       0     1     2     3          4     5     6     7            8     9     A     B           C     D     E     F       
/*0*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*1*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*2*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*3*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*4*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*5*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*6*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*7*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*8*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*9*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*A*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*B*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*C*/    true,false,false,false,     false,false,false,false,        true,true,false,false,        false,false,false,false,
/*D*/    true,false,false,false,     false,false,false,false,        true,true,false,false,        false,false,false,false,
/*E*/    false,false,false,false,    false,false,false,false,        false,false,false,false,      false,false,false,false,
/*F*/    false,false,false,false,    false,false,false,false,        false,false,false,false,      false,false,false,false
};

std::array<bool,256> disassembler_t::call_table = {
//       0     1     2     3          4     5     6     7            8     9     A     B           C     D     E     F       
/*0*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*1*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*2*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*3*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*4*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*5*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*6*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*7*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*8*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*9*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*A*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*B*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*C*/    false,false,false,false,     true,false,false,false,        false,false,false,false,      true,true,false,false,
/*D*/    false,false,false,false,     true,false,false,false,        false,false,false,false,      true,false,false,false,
/*E*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*F*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false
};

std::array<bool,256> disassembler_t::conditional_table = {
//       0     1     2     3          4     5     6     7            8     9     A     B           C     D     E     F       
/*0*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*1*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*2*/    true,false,false,false,      false,false,false,false,       true,false,false,false,       false,false,false,false,
/*3*/    true,false,false,false,      false,false,false,false,       true,false,false,false,       false,false,false,false,

/*4*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*5*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*6*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*7*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*8*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*9*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*A*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*B*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*C*/    true,false,true,false,       true,false,false,false,        true,false,true,false,        true,false,false,false,
/*D*/    true,false,true,false,       true,false,false,false,        true,false,true,false,        true,false,false,false,
/*E*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*F*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false
};

std::array<bool,256> disassembler_t::branch_table = {
//       0     1     2     3          4     5     6     7            8     9     A     B           C     D     E     F       
/*0*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*1*/    false,false,false,false,     false,false,false,false,       true,false,false,false,       false,false,false,false,
/*2*/    true,false,false,false,      false,false,false,false,       true,false,false,false,       false,false,false,false,
/*3*/    true,false,false,false,      false,false,false,false,       true,false,false,false,       false,false,false,false,

/*4*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*5*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*6*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*7*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*8*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*9*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*A*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,
/*B*/    false,false,false,false,     false,false,false,false,       false,false,false,false,      false,false,false,false,

/*C*/    true,false,true,true,        true,false,false,true,         true,true,true,false,         true,true,false,true,
/*D*/    true,false,true,false,       true,false,false,true,         true,true,true,false,         true,false,false,true,
/*E*/    false,false,false,false,     false,false,false,true,        false,true,false,false,       false,false,false,true,
/*F*/    false,false,false,false,     false,false,false,true,        false,false,false,false,      false,false,false,true
};


std::string disassembler_t::disassemble(uint8_t opc, uint16_t offset, uint16_t imm){
    if(opc != 0xCB){
        std::stringstream sstr;
        std::string str = noncb_mnemonic[opc];
        if(size_t pos = str.find('%'); pos != std::string::npos){
            char arg = str[pos+1];
            str.erase(pos,2);
            if(arg == '1'){
                std::string insert = "";
                if(labelify_table[opc])
                    insert = std::move(labelify_opc(opc, offset, imm));
                else{
                    sstr << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (imm&0xFF) << "h";
                    insert = sstr.str();
                }
                str.insert(pos, insert);
            } else if(arg == '2'){
                std::string insert = "";
                if(labelify_table[opc])
                    insert = std::move(labelify_opc(opc, offset, imm));
                else{
                    sstr << std::hex << std::setfill('0') << std::setw(4) << std::uppercase << imm << "h";
                    insert = sstr.str();
                }
                str.insert(pos, insert);
            } else if(arg == '-'){
                std::string insert = "";
                if(labelify_table[opc])
                    insert = std::move(labelify_opc(opc, offset, imm));
                else
                    insert = std::to_string(static_cast<int8_t>(imm));
                str.insert(pos, insert);
            } else{
                std::runtime_error("error disassembling. Improper string formatting.");
            }
        }
        return str;
    } else{
        return cb_mnemonic[imm&0xFF];
    }
}

std::string disassembler_t::labelify_opc(uint8_t opc, uint16_t offset, uint16_t imm){
    uint16_t adr;
    if(
        opc == 0x18 ||
        opc == 0x28 ||
        opc == 0x38 ||
        opc == 0x20 ||
        opc == 0x30
    ){
        adr = offset+static_cast<int8_t>(imm);
    } else 
        adr = imm;
    if(!labels.contains(adr)){
        std::stringstream sstr;
        sstr << std::hex << std::setfill('0') << std::setw(4) << std::uppercase << adr << "h";
        labels[adr] = static_cast<std::string>("adr_")+sstr.str();
    }
    return labels[adr];
}

uint16_t disassembler_t::get_branch_results(uint8_t opc, uint16_t offset, uint16_t imm){
    if(!is_noncb_branch(opc))
        throw std::runtime_error("tried to get branch results from non-branchable instruction");
    if(
        opc == 0x18 ||
        opc == 0x28 ||
        opc == 0x38 ||
        opc == 0x20 ||
        opc == 0x30
    ){
        return offset+static_cast<int8_t>(imm);
    } else{
        return imm;
    }
}

bool disassembler_t::is_call(uint8_t opc){
    return call_table[opc];
}

bool disassembler_t::is_ret(uint8_t opc){
    return ret_table[opc];
}

bool disassembler_t::is_conditional(uint8_t opc){
    return conditional_table[opc];
}

bool disassembler_t::is_noncb_branch(uint8_t opc){
    return branch_table[opc];
}

bool disassembler_t::is_labelifyable(uint8_t opc){
    return labelify_table[opc];
}

memory_type disassembler_t::get_memory_type(uint16_t adr){
    switch(adr)
    {
    case 0x0000 ... 0x3FFF: return memory_type::ROM0;
    case 0x4000 ... 0x7FFF: return memory_type::ROM1;
    case 0x8000 ... 0x9FFF: return memory_type::VRAM;
    case 0xA000 ... 0xBFFF: return memory_type::RAM;
    case 0xC000 ... 0xDFFF: return memory_type::WRAM;
    case 0xE000 ... 0xFDFF: return memory_type::MIRROR;;                  
    case 0xFE00 ... 0xFE9F: return memory_type::OAM;
    case 0xFEA0 ... 0xFEFF: return memory_type::ILLEGAL;
    case 0xFF00 ... 0xFF7F: return memory_type::IO;
    case 0xFF80 ... 0xFFFE: return memory_type::HRAM;
    }
    return memory_type::IO;
}

std::string disassembler_t::get_memory_region_string(uint16_t adr){
    switch(adr)
    {
    case 0x0000 ... 0x3FFF: return "rom0";
    case 0x4000 ... 0x7FFF: return "rom1";
    case 0x8000 ... 0x9FFF: return "vram";
    case 0xA000 ... 0xBFFF: return "ram";
    case 0xC000 ... 0xDFFF: return "wram";
    case 0xE000 ... 0xFDFF: return "mirror";                  
    case 0xFE00 ... 0xFE9F: return "oam";
    case 0xFEA0 ... 0xFEFF: return "illegal";
    case 0xFF00 ... 0xFF7F: return "io";
    case 0xFF80 ... 0xFFFE: return "hram";
    }
    return "io";
}