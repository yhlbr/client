#pragma once

#include "../../Shared.h"

class CORE_EXPORT SwitchRundown1MenuEvent
{
    public:
        explicit SwitchRundown1MenuEvent(bool enabled);

        bool getEnabled() const;

    private:
        bool enabled;
};
