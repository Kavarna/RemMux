#pragma once

#include "Common.h"


class IResizeable
{
public:
    virtual void resize(uint32_t rows, uint32_t cols) = 0;
};

