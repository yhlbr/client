#pragma once

#include "../../Shared.h"

class CORE_EXPORT SwitchRundownMenuEvent
{
    public:
        explicit SwitchRundownMenuEvent(bool enabled);

        bool getEnabled() const;

    private:
        bool enabled;
};
