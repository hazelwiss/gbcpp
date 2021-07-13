#include<core/instructions.h>
#include<core/instruction_defs.h>
#include<functional>

#define _FILL_PATTERN4(instr,si,inc,bytes,ticks,tickbr,a0,a1,a2,a3) \
{   array[si]       = _gen<bytes,ticks,tickbr,instr<a0>>();    \
    array[si+inc]   = _gen<bytes,ticks,tickbr,instr<a1>>();    \
    array[si+inc*2] = _gen<bytes,ticks,tickbr,instr<a2>>();    \
    array[si+inc*3] = _gen<bytes,ticks,tickbr,instr<a3>>();    \
}
#define _FILL_PATTERN3(instr,si,inc,bytes,ticks,tickbr,a0,a1,a2)    \
{   array[si]       = _gen<bytes,ticks,tickbr,instr<a0>>();    \
    array[si+inc]   = _gen<bytes,ticks,tickbr,instr<a1>>();    \
    array[si+inc*2] = _gen<bytes,ticks,tickbr,instr<a2>>();    \
}
#define _FILL_PATTERN2(instr,si,inc,bytes,ticks,tickbr,a0,a1)       \
{   array[si]       = _gen<bytes,ticks,tickbr,instr<a0>>();    \
    array[si+inc]   = _gen<bytes,ticks,tickbr,instr<a1>>();    \
}

using namespace instr_defs;

template<size_t _bytes,_instr_ticks _ticks,_instr_br_ticks _br_ticks,_cpu_function_prototype _f>
consteval cpu_function_entry _gen(){
    static_assert(_ticks%4==0,"ticks in instruction definition not divisible by 4");
    static_assert(_br_ticks%4==0,"branch ticks in instruction definition not divisible by 4");
    static_assert(_bytes < 4, "invalid bytesize for instruction");
    return std::make_tuple<size_t,_instr_ticks_entry,_cpu_function_prototype>(_bytes,{_ticks,_br_ticks},_f);
}
template<size_t dest>
consteval auto _get_switch(size_t i){
    switch(dest)
    {
    case RI::HL:
        switch(i%8){
        case 0: return _gen<1,8,8,ld_pointer_hl_r8<RI8::B>>(); 
        case 1: return _gen<1,8,8,ld_pointer_hl_r8<RI8::C>>(); 
        case 2: return _gen<1,8,8,ld_pointer_hl_r8<RI8::D>>(); 
        case 3: return _gen<1,8,8,ld_pointer_hl_r8<RI8::E>>(); 
        case 4: return _gen<1,8,8,ld_pointer_hl_r8<RI8::H>>(); 
        case 5: return _gen<1,8,8,ld_pointer_hl_r8<RI8::L>>(); 
        case 6: return _gen<1,4,4,halt>(); 
        case 7: return _gen<1,8,8,ld_pointer_hl_r8<RI8::A>>(); 
        }
        break; 
    default:
        switch(i%8){   
        case 0: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::B>>(); 
        case 1: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::C>>(); 
        case 2: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::D>>(); 
        case 3: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::E>>(); 
        case 4: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::H>>(); 
        case 5: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::L>>(); 
        case 6: return _gen<1,8,8,ld_r8_pointer_hl<(RI8)dest>>(); 
        case 7: return _gen<1,4,4,ld_r8_r8<(RI8)dest,RI8::A>>(); 
        }
    }
}
template<size_t reg, bool memory_op=false>
consteval auto _get_switch_2(size_t func){
    constexpr std::array<_cpu_function_prototype,8> func_ary = !memory_op ? 
        (decltype(func_ary)){ add_a_r8<(RI8)reg>,adc_a_r8<(RI8)reg>,sub_a_r8<(RI8)reg>,sbc_a_r8<(RI8)reg>,
            and_a_r8<(RI8)reg>,xor_a_r8<(RI8)reg>,or_a_r8<(RI8)reg>,cp_a_r8<(RI8)reg> }
        :
        (decltype(func_ary)){ add_a_pointer_hl,adc_a_pointer_hl,sub_a_pointer_hl, sbc_a_pointer_hl,
            and_a_pointer_hl,xor_a_pointer_hl,or_a_pointer_hl,cp_a_pointer_hl };
    constexpr size_t cycle_time = !memory_op ? 4:8;
    switch(func)
    {
    case 0: return _gen<1,cycle_time,cycle_time,func_ary[0]>();
    case 1: return _gen<1,cycle_time,cycle_time,func_ary[1]>();
    case 2: return _gen<1,cycle_time,cycle_time,func_ary[2]>();
    case 3: return _gen<1,cycle_time,cycle_time,func_ary[3]>();
    case 4: return _gen<1,cycle_time,cycle_time,func_ary[4]>();
    case 5: return _gen<1,cycle_time,cycle_time,func_ary[5]>();
    case 6: return _gen<1,cycle_time,cycle_time,func_ary[6]>();
    case 7: return _gen<1,cycle_time,cycle_time,func_ary[7]>();
    }
}


const _instr_array instr_table::noncb_range = []() consteval{
    _instr_array array;
    std::fill(array.begin(),array.end(),_gen<0,0,0,invalid_instr>());
    //  define some functions with no discernable pattern.
    array[0x00] = _gen<1,4,4,nop>();
    array[0x10] = _gen<2,4,4,stop>();
    array[0x20] = _gen<2,8,12,jr_cc_e8<FI::Z,UNSET>>();
    array[0x30] = _gen<2,8,12,jr_cc_e8<FI::C,UNSET>>();
    //  fill patterns
    _FILL_PATTERN4(ld_r16_n16,0x01,0x10,3,12,12,RI16::BC,RI16::DE,RI16::HL,RI16::SP);
    _FILL_PATTERN2(ld_pointer_r16_a,0x02,0x10,1,8,8,RI16::BC,RI16::DE);
    array[0x22] = _gen<1,8,8,ld_pointer_hl_increment_a>();
    array[0x32] = _gen<1,8,8,ld_pointer_hl_decrement_a>();
    _FILL_PATTERN4(inc_r16,0x03,0x10,1,8,8,RI16::BC,RI16::DE,RI16::HL,RI16::SP);
    _FILL_PATTERN3(inc_r8,0x04,0x10,1,4,4,RI8::B,RI8::D,RI8::H);
    array[0x34] = _gen<1,12,12,inc_pointer_hl>();
    _FILL_PATTERN3(dec_r8,0x05,0x10,1,4,4,RI8::B,RI8::D,RI8::H);
    array[0x35] = _gen<1,12,12,dec_pointer_hl>();
    _FILL_PATTERN3(ld_r8_n8,0x06,0x10,2,8,8,RI8::B,RI8::D,RI8::H);
    array[0x36] = _gen<2,12,12,ld_pointer_hl_n8>();
    array[0x07] = _gen<1,4,4,rlca>();
    array[0x17] = _gen<1,4,4,rla>();
    array[0x27] = _gen<1,4,4,daa>();
    array[0x37] = _gen<1,4,4,scf>();
    array[0x08] = _gen<3,20,20,ld_pointer_n16_sp>();
    array[0x18] = _gen<2,12,12,jr_e8>();
    array[0x28] = _gen<2,8,12,jr_cc_e8<FI::Z,SET>>();
    array[0x38] = _gen<2,8,12,jr_cc_e8<FI::C,SET>>();
    _FILL_PATTERN4(add_hl_r16,0x09,0x10,1,8,8,RI16::BC,RI16::DE,RI16::HL,RI16::SP);
    _FILL_PATTERN2(ld_a_pointer_r16,0x0A,0x10,1,8,8,RI16::BC,RI16::DE);
    array[0x2A] = _gen<1,8,8,ld_a_pointer_hl_increment>();
    array[0x3A] = _gen<1,8,8,ld_a_pointer_hl_decrement>();
    _FILL_PATTERN4(dec_r16,0x0B,0x10,1,8,8,RI16::BC,RI16::DE,RI16::HL,RI16::SP);
    _FILL_PATTERN4(inc_r8,0x0C,0x10,1,4,4,RI8::C,RI8::E,RI8::L,RI8::A);
    _FILL_PATTERN4(dec_r8,0x0D,0x10,1,4,4,RI8::C,RI8::E,RI8::L,RI8::A);
    _FILL_PATTERN4(ld_r8_n8,0x0E,0x10,2,8,8,RI8::C,RI8::E,RI8::L,RI8::A);
    array[0x0F] = _gen<1,4,4,rrca>();
    array[0x1F] = _gen<1,4,4,rra>();
    array[0x2F] = _gen<1,4,4,cpl>();
    array[0x3F] = _gen<1,4,4,ccf>();
    //  big block of instructions to generate inside of two for loops...
    for(size_t i = 0; i < 64; ++i){
        switch(i/8){ //  determine dest
        case 0: array[0x40+i]= _get_switch<RI::B>(i); break;
        case 1: array[0x40+i]= _get_switch<RI::C>(i); break;
        case 2: array[0x40+i]= _get_switch<RI::D>(i); break;
        case 3: array[0x40+i]= _get_switch<RI::E>(i); break;
        case 4: array[0x40+i]= _get_switch<RI::H>(i); break;
        case 5: array[0x40+i]= _get_switch<RI::L>(i); break;
        case 6: array[0x40+i]= _get_switch<RI::HL>(i); break;
        case 7: array[0x40+i]= _get_switch<RI::A>(i); break;
        }
    }
    for(size_t func = 0; func < 8; ++func){
        for(size_t reg = 0; reg < 8; ++reg){
            switch(reg){
            case 0: array[0x80+reg+func*8]=_get_switch_2<RI::B>(func);     break;
            case 1: array[0x80+reg+func*8]=_get_switch_2<RI::C>(func);     break;
            case 2: array[0x80+reg+func*8]=_get_switch_2<RI::D>(func);     break;
            case 3: array[0x80+reg+func*8]=_get_switch_2<RI::E>(func);     break;
            case 4: array[0x80+reg+func*8]=_get_switch_2<RI::H>(func);     break;
            case 5: array[0x80+reg+func*8]=_get_switch_2<RI::L>(func);     break;
            case 6: array[0x80+reg+func*8]=_get_switch_2<0,true>(func);    break;
            case 7: array[0x80+reg+func*8]=_get_switch_2<RI::A>(func);     break;
            }
        }
    }
    array[0xC0] = _gen<1,8,20,ret_cc<FI::Z,UNSET>>();
    array[0xD0] = _gen<1,8,20,ret_cc<FI::C,UNSET>>();
    array[0xE0] = _gen<2,12,12,ldh_pointer_n8_a>();
    array[0xF0] = _gen<2,12,12,ldh_a_pointer_n8>();
    _FILL_PATTERN3(pop_r16,0xC1,0x10,1,12,12,RI16::BC,RI16::DE,RI16::HL);
    array[0xF1] = _gen<1,12,12,pop_af>();
    array[0xC2] = _gen<3,12,16,jp_cc_n16<FI::Z,UNSET>>();
    array[0xD2] = _gen<3,12,16,jp_cc_n16<FI::C,UNSET>>();
    array[0xE2] = _gen<1,8,8,ldh_pointer_c_a>();
    array[0xF2] = _gen<1,8,8,ldh_a_pointer_c>();
    array[0xC3] = _gen<3,16,16,jp_n16>();
    array[0xF3] = _gen<1,4,4,di>();
    array[0xC4] = _gen<3,12,24,call_cc_n16<FI::Z,UNSET>>();
    array[0xD4] = _gen<3,12,24,call_cc_n16<FI::C,UNSET>>();
    _FILL_PATTERN4(push_r16,0xC5,0x10,1,16,16,RI16::BC,RI16::DE,RI16::HL,RI16::AF);
    array[0xC6] = _gen<2,8,8,add_a_n8>();
    array[0xD6] = _gen<2,8,8,sub_a_n8>();
    array[0xE6] = _gen<2,8,8,and_a_n8>();
    array[0xF6] = _gen<2,8,8,or_a_n8>();
    _FILL_PATTERN4(rst,0xC7,0x10,1,16,16,RST_VEC::_00,RST_VEC::_10,RST_VEC::_20,RST_VEC::_30);
    array[0xC8] = _gen<1,8,20,ret_cc<FI::Z,SET>>();
    array[0xD8] = _gen<1,8,20,ret_cc<FI::C,SET>>();
    array[0xE8] = _gen<2,16,16,add_sp_e8>();
    array[0xF8] = _gen<2,12,12,ld_hl_sp_e8>();
    array[0xC9] = _gen<1,16,16,ret>();
    array[0xD9] = _gen<1,16,16,reti>();
    array[0xE9] = _gen<1,4,4,jp_hl>();
    array[0xF9] = _gen<1,8,8,ld_sp_hl>();
    array[0xCA] = _gen<3,12,16,jp_cc_n16<FI::Z,SET>>();
    array[0xDA] = _gen<3,12,16,jp_cc_n16<FI::C,SET>>();
    array[0xEA] = _gen<3,16,16,ld_pointer_n16_a>();
    array[0xFA] = _gen<3,16,16,ld_a_pointer_n16>();
    array[0xCB] = _gen<1,4,4,cb>();
    array[0xFB] = _gen<1,4,4,ei>();
    array[0xCC] = _gen<3,12,24,call_cc_n16<FI::Z,SET>>();
    array[0xDC] = _gen<3,12,24,call_cc_n16<FI::C,SET>>();
    array[0xCD] = _gen<3,24,24,call_n16>();
    array[0xCE] = _gen<2,8,8,adc_a_n8>();
    array[0xDE] = _gen<2,8,8,sbc_a_n8>();
    array[0xEE] = _gen<2,8,8,xor_a_n8>();
    array[0xFE] = _gen<2,8,8,cp_a_n8>();
    _FILL_PATTERN4(rst,0xCF,0x10,1,16,16,RST_VEC::_08,RST_VEC::_18,RST_VEC::_28,RST_VEC::_38);
    return array;
}();

template<size_t index>
consteval RI8 determine_reg(){
    switch(index%8){
    case 0: return RI::B;
    case 1: return RI::C;
    case 2: return RI::D;
    case 3: return RI::E;
    case 4: return RI::H;
    case 5: return RI::L;  
    case 7: return RI::A;
    case 6: return RI::A;   //  don't use
    }
}

template<size_t iter=0>
consteval void fill_cb_table_bit(_instr_array& arg){ 
    constexpr RI8 reg = determine_reg<iter>();
    constexpr BIT bit = (BIT)(1<<(iter/8));
    if constexpr(iter >= 0x40)
        return;
    else 
        fill_cb_table_bit<iter+1>(arg);
    arg[0x40+iter]= iter%8==6 ? 
        _gen<2,16,16,bit_u3_pointer_hl<bit>>() 
        : _gen<2,8,8,bit_u3_r8<bit,reg>>();
}

template<size_t iter=0>
consteval void fill_cb_table_res(_instr_array& arg){
    constexpr RI8 reg = determine_reg<iter>();
    constexpr BIT bit = (BIT)(1<<(iter/8));
    if constexpr(iter >= 0x40)
        return;
    else 
        fill_cb_table_res<iter+1>(arg);
    arg[0x80+iter]= iter!=0 && iter%6==0 ? 
        _gen<2,16,16,res_u3_pointer_hl<bit>>() 
        : _gen<2,8,8,res_u3_r8<bit,reg>>();
}

template<size_t iter=0>
consteval void fill_cb_table_set(_instr_array& arg){
    constexpr RI8 reg = determine_reg<iter>();
    constexpr BIT bit = (BIT)(1<<(iter/8));
    if constexpr(iter >= 0x40)
        return;
    else 
        fill_cb_table_set<iter+1>(arg);
    arg[0xC0+iter]= iter!=0 && iter%6==0 ? 
        _gen<2,16,16,set_u3_pointer_hl<bit>>() 
        : _gen<2,8,8,set_u3_r8<bit,reg>>();
}

template<size_t iter=0>
consteval void fill_first_quarter(_instr_array& arg){
    constexpr RI8 reg = determine_reg<iter>();
    switch(iter/8)
    {
    case 0: arg[iter]=iter%8==6 ? _gen<2,16,16,rlc_pointer_hl>()  : _gen<2,8,8,rlc_r8<reg>>();  break;
    case 1: arg[iter]=iter%8==6 ? _gen<2,16,16,rrc_pointer_hl>()  : _gen<2,8,8,rrc_r8<reg>>();  break;
    case 2: arg[iter]=iter%8==6 ? _gen<2,16,16,rl_pointer_hl>()   : _gen<2,8,8,rl_r8<reg>>();   break;
    case 3: arg[iter]=iter%8==6 ? _gen<2,16,16,rr_pointer_hl>()   : _gen<2,8,8,rr_r8<reg>>();   break;
    case 4: arg[iter]=iter%8==6 ? _gen<2,16,16,sla_pointer_hl>()  : _gen<2,8,8,sla_r8<reg>>();  break;
    case 5: arg[iter]=iter%8==6 ? _gen<2,16,16,sra_pointer_hl>()  : _gen<2,8,8,sra_r8<reg>>();  break;
    case 6: arg[iter]=iter%8==6 ? _gen<2,16,16,swap_pointer_hl>() : _gen<2,8,8,swap_r8<reg>>(); break;
    case 7: arg[iter]=iter%8==6 ? _gen<2,16,16,srl_pointer_hl>()  : _gen<2,8,8,srl_r8<reg>>();  break;
    }
    if constexpr(iter >= 0x40)
        return;
    else
        fill_first_quarter<iter+1>(arg);   
}

const _instr_array instr_table::cb_range = []() consteval{
    _instr_array array;
    fill_first_quarter(array);
    fill_cb_table_bit(array);
    fill_cb_table_res(array);
    fill_cb_table_set(array);
    return array;
}();
