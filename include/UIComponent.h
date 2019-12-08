#pragma once


#include "Common.h"


class UIComponent
{
public:
    virtual void render(WINDOW*) const = 0;
};

