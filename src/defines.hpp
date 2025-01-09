#pragma once
#include <types.hpp>

#define STATIC_ASSERT(condition)                    static_assert(condition, "static_assert: "  #condition)
#define STATIC_ASSERT_MESSAGE(condition, message)   static_assert(condition, "static_assert: "  message)

enum { isInsamaritanNamespace = false };
namespace samaritan { enum { isInsamaritanNamespace = true }; }

#define BEGIN_NAMESPACE                             STATIC_ASSERT_MESSAGE(!isInsamaritanNamespace, "samaritan namespace is not previously closed"); namespace samaritan {
#define END_NAMESPACE                               } STATIC_ASSERT_MESSAGE(!isInsamaritanNamespace, "samaritan namespace is not previously opened");

#define SM_INLINE inline

