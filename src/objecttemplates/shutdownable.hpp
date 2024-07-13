#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

class Shutdownable
{
public:
    virtual void Init() = 0;
    virtual void Shutdown() = 0;
};

END_NAMESPACE