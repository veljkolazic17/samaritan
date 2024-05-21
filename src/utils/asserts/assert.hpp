#pragma once
#include <defines.hpp>

#define ASSERTS_ENABLED true

#if ASSERTS_ENABLED == true

#ifdef DEBUG
#ifdef _MSC_VER
#include <intrin.h>
#define trapBreak() __debugbreak()
#else
#define trapBreak() __builtin_trap()
#endif
#else
#define trapBreak()
#endif

enum class AssertType 
{
    SoftAssert,
    HardAssert,
    SIZE = 2
};

void show_assert(AssertType assertType, const char* file, const char* expression, unsigned int line, const char* message);

#define softAssert(condition, message)                                                          \
    do {                                                                                        \
        if (condition) {                                                                        \
            /* Do nothing if condition is true */                                               \
        } else {                                                                                \
            show_assert(AssertType::SoftAssert, __FILE__, #condition, __LINE__, message);       \
            trapBreak();                                                                        \
        }                                                                                       \
    } while(0)

#define hardAssert(condition, message)                                                          \
    do {                                                                                        \
        if (condition) {                                                                        \
            /* Do nothing if condition is true */                                               \
        } else {                                                                                \
            show_assert(AssertType::HardAssert, __FILE__, #condition, __LINE__, message);       \
            trapBreak();                                                                        \
            std::abort();                                                                       \
        }                                                                                       \
    } while(0)

#else
#define softAssert(condition, message)
#define hardAssert(condition, message)
#endif

