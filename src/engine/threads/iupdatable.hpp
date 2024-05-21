#pragma once
#if HACKS_ENABLED
#include <defines.hpp>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

class IUpdatable
{
public:
    virtual ~IUpdatable() = default;

    virtual void UpdateSingleThreaded() = 0;
    virtual void UpdateMultiThreaded() = 0;
};

END_NAMESPACE
#endif