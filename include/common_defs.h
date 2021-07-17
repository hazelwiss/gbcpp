#pragma once
#include<cstdint>
#include<cstddef>

//#define DEBUG_MODE
//
//#ifdef DEBUG_MODE
//#define __DEBUG_LINE(...) __VA_ARGS__
//#else
//#define __DEBUG_LINE(...)
//#endif

#define __DEBUG__

constexpr inline void DEBUG(auto lambda){
#ifdef __DEBUG__
    lambda();
#endif 
}

#define DEBUG_CALL(body)    \
DEBUG([&](){ body })                   

constexpr double operator"" _n(long double seconds){
    return 1000000000*seconds;
}